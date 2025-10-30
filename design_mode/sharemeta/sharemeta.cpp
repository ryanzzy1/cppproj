#include <iostream>
#include <map>
#include <vector>

using namespace std;

typedef struct  pointTag
{
    int x;
    int y;
    pointTag() {}
    pointTag(int a, int b) {
        x = a;
        y = b;
    }

    bool operator <(const pointTag& other) const
    {
        if (x < other.x) {
            return true;
        } else if (x == other.x) {
            return y < other.y;
        } 
            
        return false;        
    }
}POINT;

typedef enum PieceColorTag
{
    BLACK,
    WHITE
}PIECECOLOR;

class CPiece
{
public:
    CPiece(PIECECOLOR color) : m_color(color) {}

    PIECECOLOR Getcolor() {
        return m_color;
    }

    void Setpoint(POINT point) {
        m_point = point;
    }

    POINT Getpoint() {
        return m_point;
    }

private:
    PIECECOLOR m_color;

    POINT m_point;
};

class CGomoku : public CPiece
{
public:
    CGomoku(PIECECOLOR color) : CPiece(color) {}
};

class CPieceFactory
{
public:
    CPiece* GetPiece(PIECECOLOR color)
    {
        CPiece* pPiece = NULL;

        if (m_vecPiece.empty())
        {
            pPiece = new CGomoku(color);
            m_vecPiece.push_back(pPiece);
        } else {
            for (vector<CPiece*>::iterator it = m_vecPiece.begin(); it != m_vecPiece.end(); ++it)
            {
                if((*it)->Getcolor() == color)
                {
                    pPiece = *it;
                    break;
                }
            }
            if (pPiece == NULL)
            {
                pPiece = new CGomoku(color);
                m_vecPiece.push_back(pPiece);
            }
        }
        return pPiece;
    }

    ~CPieceFactory()
    {
        for(vector<CPiece*>::iterator it = m_vecPiece.begin(); it != m_vecPiece.end(); ++it)
        {
            if(*it != NULL)
            {
                delete *it;
                *it = NULL;
            }
        }
    }

private:
    vector<CPiece*> m_vecPiece;
};

class CChessboard
{
public:
    void Draw(CPiece *piece)
    {
        if (piece->Getcolor())
        {
            cout << "Draw a White" << "at (" << piece->Getpoint().x << "," << piece->Getpoint().y << ")" << endl;

        }
        else 
        {
            cout << "Draw a Black" << "at (" << piece->Getpoint().x << "," << piece->Getpoint().y << ")" << endl;

        }
        m_mapPieces.insert(pair<POINT, CPiece*>(piece->Getpoint(), piece));
    }

void ShowAllPieces()
{
    for(map<POINT, CPiece*>::iterator it = m_mapPieces.begin(); it != m_mapPieces.end(); ++it)
    {
        if (it->second->Getcolor())
        {
            cout << "(" << it->first.x << "," << it->first.y << ") has a white chese." << endl;
        }
        else
        {
            cout << "(" << it->first.x << "," << it->first.y << ") has a Black chese." << endl;
        }
    }
}

private:
    map<POINT, CPiece*> m_mapPieces;

};

int main()
{
    CPieceFactory *pPieceFactory = new CPieceFactory();
    CChessboard *pCheseboard = new CChessboard();

    // 1st player
    CPiece *pPiece = pPieceFactory->GetPiece(WHITE);
    pPiece->Setpoint(POINT(2, 3));
    pCheseboard->Draw(pPiece);

    // 2nd player
    pPiece = pPieceFactory->GetPiece(BLACK);
    pPiece->Setpoint(POINT(4, 5));
    pCheseboard->Draw(pPiece);

    //
    pPiece = pPieceFactory->GetPiece(WHITE);
    pPiece->Setpoint(POINT(2, 4));
    pCheseboard->Draw(pPiece);

    pPiece = pPieceFactory->GetPiece(BLACK);
    pPiece->Setpoint(POINT(3, 5));
    pCheseboard->Draw(pPiece);

    cout << "Show all cheses" << endl;
    pCheseboard->ShowAllPieces();

    if (pCheseboard != NULL)
    {
        delete pCheseboard;
        pCheseboard = NULL;
    }
    if (pPieceFactory != NULL)
    {
        delete pPieceFactory;
        pPieceFactory = NULL;
    }
}