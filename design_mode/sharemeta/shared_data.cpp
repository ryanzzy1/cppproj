#include <iostream>
#include <vector>
#include <map>

using namespace std;

typedef struct pointTag
{
    int x;
    int y;
    pointTag(){}
    pointTag(int a, int b) : x(a), y(b) {}

    bool operator <(const pointTag& other) const
    {
        if (x < other.x) {
            return true;
        } else if (x == other.x){
            return y < other.y;
        }

        return false;
    }
}POINT;

typedef enum class pieceColor : uint16_t
{
    BLACK,
    WHITE
}PIECECOLOR;

class piece
{
public:

    piece(PIECECOLOR color) : m_pieceColor(color) {}

    PIECECOLOR getColor()
    {
        return m_pieceColor;
    }

    void setPoint(POINT point)
    {
        m_piecePoint = point;
    }

    POINT getPoint()
    {
        return m_piecePoint;
    }

private:

    PIECECOLOR m_pieceColor;

    POINT m_piecePoint;
};

class pieceDerive : public piece
{
public:
    pieceDerive(PIECECOLOR color) : piece(color) {}
};

class PieceFactory
{
public:

    piece* GetPiece(PIECECOLOR color)
    {
        piece* pPiece = nullptr;

        if (m_piece.empty()) // true if empty
        {
            pPiece = new pieceDerive(color);

            m_piece.push_back(pPiece);

        } else {

            for (vector<piece*>::iterator it = m_piece.begin(); it != m_piece.end(); ++it)
            {
                if ((*it)->getColor() == color) 
                {
                    pPiece = *it;

                    break;
                }
            }

            if(pPiece == nullptr)
            {
                pPiece = new pieceDerive(color);

                m_piece.push_back(pPiece);
            }
        }

        return pPiece;
    }

    ~PieceFactory()
    {
        for (vector<piece*>::iterator it = m_piece.begin(); it != m_piece.end(); ++it)
        {
            if (*it != nullptr)
            {
                delete *it;
                *it = nullptr;
            }
        }
    }

private:
    vector<piece*> m_piece;
};

class Chessboard
{
public:
    void Draw(piece *pPiece)
    {
        if (pPiece->getColor() == PIECECOLOR::WHITE)
        {
            cout << "Draw a White" << "at (" << pPiece->getPoint().x << "," << pPiece->getPoint().y << ")" << endl;

        } 
        else 
        {
            cout << "Draw a Black" << "at (" << pPiece->getPoint().x << "," << pPiece->getPoint().y << ")" << endl;

        }

        m_mapPieces.insert(pair<POINT, piece*>(pPiece->getPoint(), pPiece));
    }

    void ShowAllPieces()
    {
        for(map<POINT, piece*>::iterator it = m_mapPieces.begin(); it != m_mapPieces.end(); ++it)
        {
            if (it->second->getColor() == PIECECOLOR::WHITE)
            {
                cout << "(" << it->first.x << "," << it->first.y << ") has a white chess." << endl; 
            }
            else 
            {
                cout << "(" << it->first.x << "," << it->first.y << ") has a black chess." << endl; 
            }
        }
    }

private:
    map<POINT, piece*> m_mapPieces;
};

int main()
{
    PieceFactory factory;
    Chessboard board;

    // 1st player
    piece* whitePiece = factory.GetPiece(PIECECOLOR::WHITE);
    whitePiece->setPoint(POINT(1, 1));
    board.Draw(whitePiece);

    // 2nd player
    piece* blackPiece = factory.GetPiece(PIECECOLOR::BLACK);
    blackPiece->setPoint(POINT(2, 2));
    board.Draw(blackPiece);

    // 3rd player
    piece* anotherWhitePiece = factory.GetPiece(PIECECOLOR::WHITE);
    anotherWhitePiece->setPoint(POINT(3, 3));
    board.Draw(anotherWhitePiece);  

    // 4th player
    piece* anotherBlackPiece = factory.GetPiece(PIECECOLOR::BLACK);
    anotherBlackPiece->setPoint(POINT(4, 4));
    board.Draw(anotherBlackPiece);

    board.ShowAllPieces();

    return 0;
}