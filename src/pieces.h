#ifndef CHESSWRAP_PIECES_H
#define CHESSWRAP_PIECES_H

#include <vector>
#include <set>
#include <string>

#include "SDL_render.h"

#include "global.h"

enum class PieceColor
{
	eWhite,
	eBlack
};

// maybe will use, we'll see
enum class PieceType
{
	ePawn,
	eBishop,
	eRook,
	eQueen,
	eKing,
	eKnight
};

PieceColor opposingColor(PieceColor c);

struct Piece {

	template <typename Derived>
	static inline SDL_Texture* blackTex = nullptr;
	template <typename Derived>
	static inline SDL_Texture* whiteTex = nullptr;

	Piece(BoardType& board, PieceColor color, BoardPos startPos, 
		unsigned int& turnCounter);

	virtual ~Piece() = default;

	// this split only needs to exist because of pawns ugh
	virtual std::set<BoardPos> attackMoves() const;
	virtual std::set<BoardPos> movementMoves() const = 0;
	std::set<BoardPos> possibleMoves() const;

	void drawTexture(SDL_Renderer* renderer, SDL_Texture* tex);
	virtual void render(SDL_Renderer* renderer);
	void setPos(BoardPos);
	bool hasMoved() const noexcept;
	PieceColor color() const noexcept;
	const BoardPos& pos() const noexcept;
	const BoardPos& lastPos() const noexcept;
	BoardType& board() const noexcept;
	virtual PieceType pieceType() const noexcept = 0;
	unsigned int lastMoveTurn() const noexcept;
	//const BoardType& board() const noexcept;

	template <typename Derived>
	SDL_Texture* texture()
	{
		return m_color == PieceColor::eBlack ? blackTex<Derived> : whiteTex<Derived>;
	}

	template <typename Derived>
	void loadTextures(SDL_Renderer* r, const std::string& pieceName)
	{
		auto surfb = SDL_LoadBMP(("resources/" + pieceName + "b.bmp").c_str());
		auto surfw = SDL_LoadBMP(("resources/" + pieceName + "w.bmp").c_str());
		if (!surfb || !surfw) throw TextureNotLoaded();
		blackTex<Derived> = SDL_CreateTextureFromSurface(r, surfb);
		whiteTex<Derived> = SDL_CreateTextureFromSurface(r, surfw);
		SDL_FreeSurface(surfw);
		SDL_FreeSurface(surfb);
	}

	friend std::set<BoardPos> possibleBishoplikeMoves(const Piece&);
	friend std::set<BoardPos> possibleRooklikeMoves(const Piece&);
protected:

	BoardPos m_pos;
	BoardPos m_lastPos;
	BoardType& m_board;
	PieceColor m_color;
	const unsigned int& m_turnCounter;
	unsigned int m_lastMoveTurn = 0;

	bool m_hasMoved = false;
};

class Pawn : public Piece {
public:
	//static inline SDL_Texture* blackTex = nullptr;
	//static inline SDL_Texture* whiteTex = nullptr;
	//Pawn(const BoardType& board, BoardPos pos);
	using Piece::Piece;

	std::set<BoardPos> attackMoves() const;
	std::set<BoardPos> movementMoves() const;

	void render(SDL_Renderer* renderer);
	PieceType pieceType() const noexcept { return PieceType::ePawn; }

private:
	//SDL_Texture* texture();
	//static void loadTextures(SDL_Renderer* r);
};

class Rook : public virtual Piece
{
public:
	//static inline SDL_Texture* blackTex = nullptr;
	//static inline SDL_Texture* whiteTex = nullptr;
	using Piece::Piece;
	//Rook(const BoardType& board, BoardPos pos);

	std::set<BoardPos> movementMoves() const;
	void render(SDL_Renderer* renderer);
	PieceType pieceType() const noexcept { return PieceType::eRook; }
private:
	//SDL_Texture* texture();
	//static void loadTextures(SDL_Renderer* r);
};

class Bishop : public virtual Piece
{
public:
	//static inline SDL_Texture* blackTex = nullptr;
	//static inline SDL_Texture* whiteTex = nullptr;
	using Piece::Piece;

	std::set<BoardPos> movementMoves() const;
	void render(SDL_Renderer* renderer);
	PieceType pieceType() const noexcept { return PieceType::eBishop; }
private:
	//SDL_Texture* texture();
	//static void loadTextures(SDL_Renderer* r);
};

class Knight : public Piece
{
public:
	using Piece::Piece;

	std::set<BoardPos> movementMoves() const;
	void render(SDL_Renderer* renderer);
	PieceType pieceType() const noexcept { return PieceType::eKnight; }
};

class Queen : public Piece
{
public:
	using Piece::Piece;

	std::set<BoardPos> movementMoves() const;
	void render(SDL_Renderer*);
	PieceType pieceType() const noexcept { return PieceType::eQueen; }
};

class King : public Piece
{
public:
	using Piece::Piece;

	std::set<BoardPos> movementMoves() const;
	void render(SDL_Renderer*);
	PieceType pieceType() const noexcept { return PieceType::eKing; }

	enum class CastleDirection
	{
		eLeft,
		eRight,
		eBoth
	};

	static bool wouldBeChecked(const Piece&, std::pair<int, int> posOffset = { 0, 0 });
	static bool canCastle(const Piece&, CastleDirection);
};

#endif // CHESSWRAP_PIECES_H