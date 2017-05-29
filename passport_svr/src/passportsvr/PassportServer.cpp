//
//  PassportServer.cpp
//  passport_svr
//
//  Created by Wayne on 6/9/15.
//  Copyright (c) 2015 viichi.com. All rights reserved.
//

#include "PassportServer.h"

#include "common/LogPrinter.h"
#include "database/PassPortDatabaseConnection.h"


#include "vnVariableScript.h"
#include "curl/CurlWrapper.h"
#include "curl/CurlManager.h"

#include <thread>
#include <iostream>

#include "adminlisten/AdminListen.h"
#include "clientlisten/ClientListen.h"
#include "gamesvrlisten/GamesvrListen.h"
#include "manager/DirtyManager.h"
#include "manager/DirtyManager2.h"
#include "manager/NicknameManager.h"
#include "manager/PlayerManager.h"
#include "manager/RsaKeyManager.h"
#include "manager/SessionManager.h"
#include "manager/TimerManager.h"
#include "tranbase/notifymanager/NotifyMgr.h"
#include "transaction/GSTransaction.h"
#include "transaction/GSTransactionMgr.h"
#include "transaction/GSTransContext.h"
#include "transaction/notifymanager/GSNotifyMgr.h"
#include "WorldManager.h"
#include "vnBase64.h"
#include "tool/crypt/Crypt.h"
#include "vnVariableAccessor.h"

PassPortDatabaseWorkerPool PassPortDatabase;

IMPL_SERVER_APP(PassportServer)

bool PassportServer::initialize(int argc, const char *argv[]) {
	/*  if (!sWorldManager.loadBaseConfig()) {
		  return false;
	  }*/

    if (!sWorldManager.m_console_mode) {
        Log::instance().printer(vnnew LogPrinterFile("logs"));
    }

    m_linkPool = LinkPool::create(sWorldManager.m_thread_count);
	if (!m_linkPool) {
		return false;
	}
    VN_LOG_DEBUG("LinkPool init OK! ");

    sWorldManager.SetLinkPool(m_linkPool);

	/*if (DirtyManager2::instance().init() != true) {
		VN_LOG_FATAL("DirtyManager2 init FAILED!");
		return false;
	}
	VN_LOG_DEBUG("DirtyManager2 init OK! ");

	if (!NicknameManager::instance().init()) {
		VN_LOG_FATAL("NicknameManager init FAILED!");
		return false;
	}
	VN_LOG_DEBUG("NicknameManager init OK! ");*/

	sCurlManager.init(m_linkPool,90000);
	VN_LOG_DEBUG("CurlManager init OK! ");

	/*sPlayerManager.init();
	VN_LOG_DEBUG("PlayerManager init OK! ");*/

	/*m_clientListen = m_linkPool->listen(&ClientListen::create, NetAddress(0, sWorldManager.m_client_port), true);
	if (!m_clientListen) {
		VN_LOG_FATAL("listen *:" << sWorldManager.m_client_port << " FAILED!");
		return false;
	}
	VN_LOG_DEBUG("Client Listener init OK! ");

	m_adminListen = m_linkPool->listen(&AdminListen::create, NetAddress(0, sWorldManager.m_admin_port), true);
	if (!m_adminListen) {
		VN_LOG_FATAL("listen *:" << sWorldManager.m_admin_port << " FAILED!");
		return false;
	}
	VN_LOG_DEBUG("Admin Listener init OK! ");

	 m_gamesvrListen= m_linkPool->listen(&GamesvrListen::create, NetAddress(0, sWorldManager.m_gamesvr_port), true);
	if (!m_gamesvrListen) {
		VN_LOG_FATAL("listen *:" << sWorldManager.m_gamesvr_port << " FAILED!");
		return false;
	}
	sWorldManager.SetGamesvrListen(m_gamesvrListen);
	VN_LOG_DEBUG("Gamesvr Listener init OK! ");



	if (!m_linkPool->listen([this]() {return &m_http_listen;}, { 0, sWorldManager.m_http_port }, false)) {
		VN_LOG_ERROR("can NOT listen *:" << sWorldManager.m_http_port);
		return false;
	}
	VN_LOG_DEBUG("Http Listener init OK! ");
*/

    if (sWorldManager.m_console_mode) {
		std::thread th([this]() {
			VN_LOG_INFO("console thread start");
			str8 line;
			VariableScript vs;
			for (;;) {
				std::getline(std::cin, line);
				if (!line.empty()) {
					auto cmds = vs.parse(line.c_str(), line.length(), nullptr);
					if (cmds && !cmds->empty()) {
						if (!_console_command(std::move(cmds))) {
							break;
						}
					} else {
						VN_LOG_ERROR("Invalid Console Command(s)!");
					}
				}
			}
		});
		th.detach();
	}

	VN_LOG_INFO("Passport start OK");
	return true;
}

void PassportServer::finalize() {
    if (m_clientListen) {
        m_clientListen->close();
    }
    if (m_adminListen) {
    	m_adminListen->close();
    }
    if (m_gamesvrListen) {
    	m_gamesvrListen->close();
    }
	m_http_listen.close();
    
    //SessionManager::instance().closeAll();
	sCurlManager.Destroy();
    if (m_linkPool) {
        m_linkPool->release();
    }
    _stopDB();
    VN_LOG_INFO("stop.");
}


RandomEngine _randomEngine;
bool PassportServer::_console_command(RefCountedPtr<Variable_object> cmds) {
    auto cmd = cmds->at(0).dyn_cast<Variable_id>();
    if (!cmd) {
        return true;
    }
    if (cmd->value() == "exit") {
        quit();
        return false;
    }
	if (cmd->value() == "send2") {
		s32 count = cmds->at(1).dyn_cast<Variable_int32>()->value();
		s32 threadscount = cmds->at(2).dyn_cast<Variable_int32>()->value();
		std::function<void()> thread_task = [this,count]() {
			for (int i = 0; i < count; i++) {
				//https://rest.gotye.com.cn/api/GetFriends
				HttpRequestPtr aReqPtr(new HttpRequestC);
				aReqPtr->addr.from_string("61.135.169.121:443");
				aReqPtr->host = "www.baidu.com";
				aReqPtr->content = "/s?wd=qwe&rsv_spt=1&rsv_iqid=0xad7d4a690000e1f4&issp=1&f=8&rsv_bp=0&rsv_idx=2&ie=utf-8&tn=baiduhome_pg&rsv_enter=1&rsv_sug3=3&rsv_sug1=1";
				aReqPtr->isPost = false;
				aReqPtr->isHttps = true;

				aReqPtr->callBackFn = [](const HttpResponse& _httpResponse ) {
					BufferPtr pBuffer = _httpResponse.content();
					VN_LOG_DEBUG("receive size"<<pBuffer->size());
				};
				sCurlManager.CurlSend(aReqPtr);
			}
		};

		std::thread threads[threadscount];
		for (int i = 0; i < threadscount; i++) {
			threads[i] = std::thread(thread_task);
		}
		VN_LOG_DEBUG( "Done spawning threads! Now wait for them to join\n");
		for (auto& t : threads) {
			t.join();
		}
		VN_LOG_DEBUG( "All threads joined.\n");
		return true;
	}
    if(cmd->value() == "send"){
    	//https://rest.gotye.com.cn/api/GetFriends
		//https://tieba.baidu.com/index.html
		HttpRequestPtr aReqPtr(new HttpRequestC);
		aReqPtr->host = "developer.51cto.com";
		aReqPtr->content = "/art/201612/523652.htm";
		//aReqPtr->isPost = true;
		//aReqPtr->isHttps = true;
		//测试生命周期
		HttpRequestPtr testprt(new HttpRequestC);
		testprt->content = "testprt";
		aReqPtr->callBackFn = [testprt](const HttpResponse& _httpResponse ) {
			VN_LOG_DEBUG("testprt" << testprt->content);
			BufferPtr pBuffer = _httpResponse.content();
			VN_LOG_DEBUG("Rev : "<<(const char *)pBuffer->lock());
			vn::RefCountedPtr<vn::Variable_object>	vJSONPtr = _getJsonObj(_httpResponse);
			if (vJSONPtr.get()) {

				/*vn::RefCountedPtr<vn::Variable_object>	entities = vJSONPtr->queryAttributeObject("entities");
				if (entities.get()) {
					VN_LOG_DEBUG("entities.get() ");
					for(size_t i =0; i< entities->size();i++){
						vn::RefCountedPtr<vn::Variable_object> entity =	entities->at(i);
					}

				}*/
			}
		};

		sCurlManager.CurlSend(aReqPtr);
		 return true;
    }
    return true;
}
bool PassportServer::_startDB(){
	 mysql_library_init(-1, NULL, NULL);
	 if (!PassPortDatabase.Open(sWorldManager.m_sDBInfo, sWorldManager.m_dbThreads)) {
		VN_LOG_FATAL("Cannot connect to database");
		return false;
	}
	return true;
}
bool PassportServer::_stopDB(){
	PassPortDatabase.Close();
	 mysql_library_end();
	return true;
}

void PassportServer::update(f32 deltaTime){
	SGSTransactionMgr.Process();
	SNotifyManager.Process();
	sTimerManager.updateTimers(deltaTime);
}
