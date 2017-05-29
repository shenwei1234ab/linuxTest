/*******************************************************************************
* Copyright(C) Tencent 2012
********************************************************************************/
#ifndef SRC_UTILLOGIC_TRANSACTIONMGR_TRANSACTION_H_
#define SRC_UTILLOGIC_TRANSACTIONMGR_TRANSACTION_H_

#include <set>

#include "tranbase/transactionmgr/TransTimer.h"
#include "tranbase/transactionmgr/IAction.h"
class TransactionMgr;


struct ActionOp
{
    ActionOp()
        :m_pAction(NULL)
    {

    }
    ActionOp(IAction* pAction)
        :m_pAction(pAction)
    {

    }
    IAction* m_pAction;
};
class Transaction
{ 
public: 
	typedef std::vector<int> CBVec;
    typedef std::vector<ActionOp> ActionOpVec;


	explicit Transaction(TransactionMgr& mgr,time_t span);
	virtual ~Transaction();
    virtual uint32_t GetId() const = 0;
	const char* GetString() const {return GetMacroName(GetId());};
    // �������񣬵���������ӵ�TransactionMgr��ʱ��������
    virtual int OnStart();

    // ����ʱ�ӿ�
	virtual int OnTimeOut();

    // ����ʧ��
	virtual int OnFail();

    // �������
	virtual int OnEnd();

    // ��ĳ�������ʱ��֪ͨ
	virtual int OnActionEnd();
	
	// ��������״̬ʱ����
	virtual int OnReset();

	// �������ʱ�����۳ɰܶ������
	virtual int OnExeTransEnd() { return 0; }

	// �������ʱ�����۳ɰܶ������
	virtual int OnExeTransFini() { return 0; }

	// �Ƿ�֧������
	virtual bool Reentrant() { return true; }

	void StartTimer(){ m_TransTimer.StartTimer(); };
	void CloseTimer(){ m_TransTimer.CloseTimer(); };


    // ��ȡ�������id
    uint64_t GetTransactionId() const { return m_iIdx; }
    // Ϊĳ���ȡ��һ��callbackid
	int RequestCallbackId(IAction* act);
    //���ǰ���callback��Ϣ
	void ClearCallback();
    
    // Action��ӵ�Transaction����ִ��˳���������
    size_t AddAction(IAction*);
    int ClearAction();
    IAction* GetAction( uint32_t idx);
    ActionOp* GetActionOp( uint32_t idx);
    // ������һ������һ�鲢�У�����
    int StartNextActions();

    int AddWaitingResActionCount();

    // ����ʼ
    int DoStart();
    // ����ʧ��
    int DoFail();
    // �������
    int DoEnd();
	// ����ǿ�н������߼����ж�ʧ����
	int DoForceEnd();
    // ����ʱ
    int DoTimeOut();
    // ������һ�������
    int DoActionEnd();
	// ��������״̬
	int DoRestart();

	// ���ý��
	virtual void SetResult(int);
	virtual int GetResult();

	void AddAllAction()
	{
		_AddPreAction();
		_AddAction();
		_AddPostAction();
	};
	//�ڵ���action response֮ǰ�����������
	virtual void PrepareResponse(){};

	TransactionMgr & GetMgr() const
	{
		return m_TransMgr;
	}

protected: 
	// �����������Ҫaction
	virtual void _AddAction(){};
	// ��������Ԥ����action
	virtual void _AddPreAction(){};
	// �������������Ҫ����actoin
	virtual void _AddPostAction(){};
    //IActionReturn _StartSingleAction(uint32_t idx);
	TransactionMgr& m_TransMgr;
    int m_iNextPhysicIdx;
    //int m_iWaitingCount;
    uint64_t m_iIdx;
	CBVec m_CBVec;
	TransTimer m_TransTimer;
    ActionOpVec m_ActionOpVec;
}; 
#endif // end of SRC_UTILLOGIC_TRANSACTIONMGR_TRANSACTION_H_
