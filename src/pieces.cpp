#include "pieces.h"

#include <iostream>
#include <set>
#include <vector>

#include "global.h"

PieceColor opposingColor(PieceColor c)
{
	return c == PieceColor::eBlack ? PieceColor::eWhite : PieceColor::eBlack;
}

Piece::Piece(BoardType& board, PieceColor color, BoardPos startPos,
	unsigned int& turnCounter)
	: m_pos{ startPos }, m_board{ board }, m_color{ color }, 
	m_lastPos{ m_pos }, m_turnCounter{ turnCounter }, m_lastMoveTurn{ 0 }
{ }


std::set<BoardPos> Piece::attackMoves() const
{
	return movementMoves();
}

std::set<BoardPos> Piece::possibleMoves() const
{
	std::set<BoardPos> result;
	auto&& attackMoves = this->attackMoves();
	for (auto& move : attackMoves)
	{
		auto& [x, y] = move;
		if (m_board[x][y].piece && m_board[x][y].piece->color() != m_color)
			result.insert(move);

		// en passant check
		int sign = m_color == PieceColor::eWhite ? -1 : 1;
		if (pieceType() == PieceType::ePawn && 
			!m_board[x][y].piece && 
			m_board[x][BoardY{ y + sign }].piece &&
			m_board[x][BoardY{ y + sign }].piece->pieceType() == PieceType::ePawn &&
			m_board[x][BoardY{ y + sign }].piece->color() == opposingColor(m_color) &&
			m_board[x][BoardY{ y + sign }].piece->lastPos() == BoardPos{ x, y - sign } &&
			m_board[x][BoardY{ y + sign }].piece->lastMoveTurn() == m_turnCounter - 1)
		{
			result.insert(move);
		}
	}
	//result.merge(attackMoves());
	result.merge(movementMoves());

	std::set<BoardPos> filteredByCheck;
	std::shared_ptr<Piece> king;
	for (auto& col : m_board)
		for (auto& cell : col)
		{
			if (cell.piece && cell.piece->pieceType() == PieceType::eKing && 
				cell.piece->color() == m_color)
				king = cell.piece;
		}
	if (!king) throw std::runtime_error("you killed the king apparently? this is a bug, please don't do that, but tell me how you did it so i can make that not happen");
	for (auto& move : result)
	{
		BoardType& board = king->board();
		auto toReplacePtr = board[move.first][move.second].piece;
		auto toMovePtr = board[pos().first][pos().second].piece;
		BoardPos moveFrom = pos();

		board[moveFrom.first][moveFrom.second].piece.reset();
		board[move.first][move.second].piece = toMovePtr;
		toMovePtr->m_pos = move;

		bool wouldCheck = King::wouldBeChecked(*king);

		board[moveFrom.first][moveFrom.second].piece = toMovePtr;
		board[move.first][move.second].piece = toReplacePtr;
		toMovePtr->m_pos = moveFrom;
		if (!wouldCheck)
			filteredByCheck.insert(move);
	}

	return filteredByCheck;
}

void Piece::drawTexture(SDL_Renderer* r, SDL_Texture* tex)
{
	auto&& [x, y] = boardToWindowCoords(m_pos);
	SDL_Rect pawnRect{ x, y - cell_h, cell_w, cell_h };
	SDL_RenderCopy(r, tex, NULL, &pawnRect);
}

void Piece::render(SDL_Renderer* r)
{
	SDL_SetRenderDrawColor(r, UINT8_MAX, 0, 0, UINT8_MAX);
	auto cx = (cell_w / 2) + boardToWindowX(m_pos.first.get());
	auto cy = boardToWindowY(m_pos.second.get()) - cell_h / 2;
	//auto&& [x, y] = boardToWindowCoords(m_pos);
	SDL_Rect pawnRect{ cx - cell_w / 10, cy - cell_h/10, cell_w/5, cell_h/5 };
	SDL_RenderFillRect(r, &pawnRect);
}
void Piece::setPos(BoardPos pos)
{
	m_pos = pos;
	m_hasMoved = true;
	m_lastMoveTurn = m_turnCounter;
}
bool Piece::hasMoved() const noexcept
{
	return m_hasMoved;
}

PieceColor Piece::color() const noexcept
{
	return m_color;
}
const BoardPos& Piece::pos() const noexcept
{
	return m_pos;
}
const BoardPos& Piece::lastPos() const noexcept
{
	return m_lastPos;
}
BoardType& Piece::board() const noexcept
{
	return m_board;
}
//const BoardType& Piece::board() const noexcept
//{
//	return m_board;
//}
unsigned int Piece::lastMoveTurn() const noexcept
{
	return m_lastMoveTurn;
}

std::set<BoardPos> Pawn::attackMoves() const
{
	std::set<BoardPos> attacks;
	int sign = m_color == PieceColor::eWhite ? 1 : -1;

	BoardPos attackChecks[] = {
		{m_pos.first + 1, m_pos.second + 1 * sign},
		{m_pos.first - 1, m_pos.second + 1 * sign}
	};
	for (auto& pos : attackChecks)
	{
		attacks.insert(pos);
	}

	return attacks;
}

std::set<BoardPos> Pawn::movementMoves() const
{
	std::set<BoardPos> moves{};
	int sign = m_color == PieceColor::eWhite ? 1 : -1;

	// can only move if not blocked
	BoardPos forwardCheck = { m_pos.first, m_pos.second + 1 * sign };
	if (!m_board[forwardCheck.first][forwardCheck.second].piece)
	{
		moves.insert({ m_pos.first, m_pos.second + 1 * sign });
		if (!m_hasMoved)
		{
			BoardPos lookahead2 = { m_pos.first, m_pos.second + 2 * sign };
			if (!m_board[lookahead2.first][lookahead2.second].piece)
				moves.insert({ m_pos.first, m_pos.second + 2 * sign });
		}
	}

	return moves;
}

void Pawn::render(SDL_Renderer* r)
{
	if (!texture<Pawn>())
		loadTextures<Pawn>(r, "pawn");
	drawTexture(r, texture<Pawn>());
}

//SDL_Texture* Pawn::texture()
//{
//	return m_color == PieceColor::eBlack ? blackTex : whiteTex;
//}
//
//void Pawn::loadTextures(SDL_Renderer* r)
//{
//	auto surfb = SDL_LoadBMP("resources/pawnb.bmp");
//	auto surfw = SDL_LoadBMP("resources/pawnw.bmp");
//	blackTex = SDL_CreateTextureFromSurface(r, surfb);
//	whiteTex = SDL_CreateTextureFromSurface(r, surfw);
//
//	SDL_FreeSurface(surfw);
//	SDL_FreeSurface(surfb);
//}


//Rook::Rook(const BoardType& board, BoardPos pos)
//	: m_board{ board }, Piece{ pos }
//{ }

std::set<BoardPos> Rook::movementMoves() const
{
	return possibleRooklikeMoves(*this);
}
void Rook::render(SDL_Renderer* r)
{
	if (!texture<Rook>())
		loadTextures<Rook>(r, "rook");
	drawTexture(r, texture<Rook>());
}

//SDL_Texture* Rook::texture()
//{
//	return m_color == PieceColor::eBlack ? blackTex : whiteTex;
//}
//
//void Rook::loadTextures(SDL_Renderer* r)
//{
//	auto surfb = SDL_LoadBMP("resources/rookb.bmp");
//	auto surfw = SDL_LoadBMP("resources/rookw.bmp");
//	blackTex = SDL_CreateTextureFromSurface(r, surfb);
//	whiteTex = SDL_CreateTextureFromSurface(r, surfw);
//
//	SDL_FreeSurface(surfw);
//	SDL_FreeSurface(surfb);
//}


std::set<BoardPos> Bishop::movementMoves() const
{
	return possibleBishoplikeMoves(*this);
}

void Bishop::render(SDL_Renderer* r)
{
	if (!texture<Bishop>())
		loadTextures<Bishop>(r, "bishop");
	drawTexture(r, texture<Bishop>());
}

//SDL_Texture* Bishop::texture()
//{
//	return m_color == PieceColor::eBlack ? blackTex : whiteTex;
//}
//
//void Bishop::loadTextures(SDL_Renderer* r)
//{
//	auto surfb = SDL_LoadBMP("resources/bishopb.bmp");
//	auto surfw = SDL_LoadBMP("resources/bishopw.bmp");
//	blackTex = SDL_CreateTextureFromSurface(r, surfb);
//	whiteTex = SDL_CreateTextureFromSurface(r, surfw);
//
//	SDL_FreeSurface(surfw);
//	SDL_FreeSurface(surfb);
//}


std::set<BoardPos> Knight::movementMoves() const
{
	std::set<BoardPos> result;
	int xSigns[] = { 1, -1 };
	int ySigns[] = { 1, -1 };
	// whether the motion of one cell is vertical or horizontal
	bool oneMoveIsVertical[] = { true, false };

	for (auto xSign : xSigns)
		for (auto ySign : ySigns)
			for (auto oneIsVertical : oneMoveIsVertical)
			{
				int dx = oneIsVertical ? 2 : 1;
				int dy = oneIsVertical ? 1 : 2;
				dx *= xSign;
				dy *= ySign;
				BoardPos target = { m_pos.first + dx, m_pos.second + dy };
				if (auto& piece = m_board[target.first][target.second].piece)
					if (piece->color() == m_color)
						continue;
				result.insert(target);
			}

	return result;
}
void Knight::render(SDL_Renderer* renderer)
{
	if (!texture<Knight>())
		loadTextures<Knight>(renderer, "knight");
	drawTexture(renderer, texture<Knight>());
}



std::set<BoardPos> Queen::movementMoves() const
{
	std::set<BoardPos> result;
	result.merge(possibleBishoplikeMoves(*this));
	result.merge(possibleRooklikeMoves(*this));
	return result;
}
void Queen::render(SDL_Renderer* renderer)
{
	if (!texture<Queen>())
		loadTextures<Queen>(renderer, "quen");
	drawTexture(renderer, texture<Queen>());
}


std::set<BoardPos> King::movementMoves() const
{
	std::set<BoardPos> result;
	for (int dx = -1; dx <= 1; ++dx)
	{
		for (int dy = -1; dy <= 1; ++dy)
		{
			BoardX x = m_pos.first + dx;
			BoardY y = m_pos.second + dy;
			if ((dx != 0 || dy != 0))// && !wouldBeChecked(*this, { dx, dy }))
			{
				if (m_board[x][y].piece && m_board[x][y].piece->color() == color())
					continue;
				result.insert({ m_pos.first + dx, m_pos.second + dy });
			}
		}
	}
	if (canCastle(*this, CastleDirection::eLeft))
		result.insert({ m_pos.first - 2, m_pos.second });
	if (canCastle(*this, CastleDirection::eRight))
		result.insert({ m_pos.first + 2, m_pos.second });
	return result;
}
void King::render(SDL_Renderer* r)
{
	if (!texture<King>())
		loadTextures<King>(r, "king");
	drawTexture(r, texture<King>());
}

bool King::wouldBeChecked(const Piece& k, std::pair<int, int> posOffset)
{
	if (k.pieceType() != PieceType::eKing)
		return false;
	auto& board = k.board();
	auto& [kx, ky] = k.pos();
	BoardPos newPos = { kx + posOffset.first, ky + posOffset.second };
	auto kingPtr = k.board()[kx][ky].piece;
	auto toReplacePtr = k.board()[newPos.first][newPos.second].piece;

	// do the move!
	board[newPos.first][newPos.second].piece = kingPtr;
	board[kx][ky].piece.reset();

	bool foundCheck = false;
	PieceColor otherColor = opposingColor(k.color());
	for (int i = 0; i < board_w; ++i)
	{
		for (int j = 0; j < board_h; ++j)
		{
			if (foundCheck) break;
			if (board[i][j].piece && 
				board[i][j].piece->color() == otherColor)
			{
				if (board[i][j].piece->pieceType() == PieceType::eKing)
				{
					const BoardPos& otherPos = { i, j };

					BoardX kx1 = kx + 1;
					BoardX kx_1 = kx - 1;
					BoardY ky1 = ky + 1;
					BoardY ky_1 = ky - 1;

					for (int dx = -1; dx <= 1; ++dx)
						for (int dy = -1; dy <= 1; ++dy)
						{
							if (dx == 0 && dy == 0) continue;
							if (kx == BoardX{ i + dx } && ky == BoardY{ j + dy })
								foundCheck = true;
						}
				}
				else if (board[i][j].piece->attackMoves().count(newPos) > 0)
					foundCheck = true;
			}
		}
	}

	// undo the move
	// ideally this should be raii but i don't have the time or energy or both
	board[kx][ky].piece = kingPtr;
	board[newPos.first][newPos.second].piece = toReplacePtr;

	return foundCheck;
}

bool King::canCastle(const Piece& k, CastleDirection dir)
{
	if (k.pieceType() == PieceType::eKing)
	{
		if (wouldBeChecked(k) || k.hasMoved())
			return false;
		auto& board = k.board();
		auto [x, y] = k.pos();
		// double check if moved or not???
		if (x != 4 || (y != 1 && y != board_h - 2) || board[x][y].piece.get() != &k)
			return false;
		// search left
		bool canCastleLeft = false;
		bool canCastleRight = false;
		if (!board[x - 1][y].piece && !board[x - 2][y].piece &&
			!board[x - 3][y].piece && board[0][y].piece &&
			board[0][y].piece->pieceType() == PieceType::eRook &&
			!board[0][y].piece->hasMoved())
		{
			canCastleLeft = true;
		}
		// search right
		if (!board[x + 1][y].piece && !board[x + 2][y].piece &&
			board[board_w - 1][y].piece &&
			board[board_w - 1][y].piece->pieceType() == PieceType::eRook &&
			!board[board_w - 1][y].piece->hasMoved())
		{
			canCastleRight = true;
		}
		switch (dir)
		{
		case CastleDirection::eLeft:
			return canCastleLeft;
		case CastleDirection::eRight:
			return canCastleRight;
		case CastleDirection::eBoth:
			return canCastleLeft && canCastleRight;
		}
	}
	return false;
}