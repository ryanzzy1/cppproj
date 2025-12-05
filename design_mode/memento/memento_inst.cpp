#include <iostream>
using namespace std;


class RoleStateMemento
{
public:
    RoleStateMemento(unsigned iBlood, unsigned iAttack, unsigned iDefense) : m_iBlood(iBlood), m_iAttack(iAttack), m_iDefense(iDefense){
        cout << "RoleStateMemento ctor called." << endl;
    }

private:
    friend class GameRole;

    unsigned GetBloodValue() {
        return m_iBlood;
    }

    unsigned GetAttackValue() {
        return m_iAttack;
    }
    unsigned GetDefenseValue() {
        return m_iDefense;
    }

    unsigned m_iBlood;
    unsigned m_iAttack;
    unsigned m_iDefense;
};

class GameRole
{
public:
    GameRole() : m_iBlood(100), m_iAttack(100), m_iDefense(100) {
        cout << "GameRole ctor called." << endl;
    }

    // store 
    RoleStateMemento *SaveState(){
        return new RoleStateMemento(m_iBlood, m_iAttack, m_iDefense);
    }

    // recovery
    void RecoveryState(RoleStateMemento *pRoleStateMemento){
        m_iBlood = pRoleStateMemento->GetBloodValue();
        m_iAttack = pRoleStateMemento->GetAttackValue();
        m_iDefense = pRoleStateMemento->GetDefenseValue();
        cout << "GameRole state recovered." << endl;
    }

    void ShowState(){
        cout << "Blood: " << m_iBlood << ", Attack: " << m_iAttack << ", Defense: " << m_iDefense << endl;
    }

    void Fight(){
        m_iBlood -= 100;
        m_iAttack -= 10;
        m_iDefense -= 20;

        if (m_iBlood == 0){
            cout << "Game Over" << endl;
        }
    }

private:
    unsigned m_iBlood;
    unsigned m_iAttack;
    unsigned m_iDefense;
};

class RoleStateCaretaker
{
public:
    void setRoleStateMemento(RoleStateMemento *pRoleStateMemento){
        m_pRoleStateMemento = pRoleStateMemento;
        cout << "RoleStateCaretaker setRoleStateMemento called." << endl;
    }

    RoleStateMemento *GetRoleStateMemento(){
        return m_pRoleStateMemento;
        cout << "RoleStateCaretaker GetRoleStateMemento called." << endl;
    }

private:
    RoleStateMemento *m_pRoleStateMemento;
};

int main()
{
    GameRole *pLiXY = new GameRole();
    pLiXY->ShowState();

    // store 
    RoleStateCaretaker *pRoleStateCaretaker = new RoleStateCaretaker();
    pRoleStateCaretaker->setRoleStateMemento(pLiXY->SaveState());

    // Fight
    pLiXY->Fight();
    pLiXY->ShowState();

    // read and start from begin
    pLiXY->RecoveryState(pRoleStateCaretaker->GetRoleStateMemento());
    pLiXY->ShowState();

    return 0;
}