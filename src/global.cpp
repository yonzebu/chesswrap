#include "global.h"

#include "pieces.h"

BoardX windowToBoardX(int windowX)
{
	return windowX / cell_w;
}

BoardY windowToBoardY(int windowY)
{
	return (board_px_h - windowY) / cell_h;
}

BoardPos windowToBoardCoords(int windowX, int windowY)
{
	return { windowToBoardX(windowX), windowToBoardY(windowY) };
}

int boardToWindowX(int boardX)
{
	return boardX * cell_w;
}

int boardToWindowY(int boardY)
{
	return (board_h - boardY) * cell_h;
}

std::pair<int, int> boardToWindowCoords(const BoardPos& pos)
{
	return { boardToWindowX(pos.first.get()), boardToWindowY(pos.second.get()) };
}

bool isOnBoard(int x, int y)
{
	return x >= 0 && y >= 0 && x <= board_px_w && y <= board_px_h;
}

// doesn't work, don't use
void invertSurfaceColors(SDL_Surface* s)
{
	SDL_LockSurface(s);
	auto pixelSize = s->format->BytesPerPixel;

	unsigned char* pixels = static_cast<unsigned char*>(s->pixels);
	for (size_t x = 0; x < s->w; ++x)
	{
		for (size_t y = 0; y < s->h; ++y)
		{
			uint32_t* pixDataPtr = reinterpret_cast<uint32_t*>(pixels + x + y * s->pitch);
			uint32_t& pixData = *pixDataPtr;

			uint32_t red, green, blue, alpha;

			red = pixData & s->format->Rmask;
			red >>= s->format->Rshift;
			red = static_cast<uint8_t>(red << s->format->Rloss);

			green = pixData & s->format->Gmask;
			green >>= s->format->Gshift;
			green = static_cast<uint8_t>(green << s->format->Gloss);

			blue = pixData & s->format->Bmask;
			blue >>= s->format->Bshift;
			blue = static_cast<uint8_t>(blue << s->format->Bloss);

			alpha = pixData & s->format->Amask;
			if (s->format->Amask)
			{
				alpha >>= s->format->Ashift;
				alpha = static_cast<uint8_t>(alpha << s->format->Aloss);
			}

			red = (UINT8_MAX - red);
			green = (UINT8_MAX - green);
			blue = (UINT8_MAX - blue);

			red >>= s->format->Rloss;
			red <<= s->format->Rshift;
			red &= s->format->Rmask;
			green >>= s->format->Gloss;
			green <<= s->format->Gshift;
			green &= s->format->Gmask;
			blue >>= s->format->Bloss;
			blue <<= s->format->Bshift;
			blue &= s->format->Bmask;
			alpha >>= s->format->Aloss;
			alpha <<= s->format->Ashift;
			alpha &= s->format->Amask;

			pixData = red | green | blue | alpha;
		}
	}


	SDL_UnlockSurface(s);
}

std::set<BoardPos> possibleBishoplikeMoves(const Piece& p)
{
	const BoardPos& start = p.pos();
	const BoardType& board = p.board();
	const PieceColor& color = p.color();
	std::set<BoardPos> result;

	BoardPos searchPos = { start.first + 1, start.second + 1 };
	while (searchPos != start)
	{
		if (board[searchPos.first][searchPos.second].piece)
		{
			if (board[searchPos.first][searchPos.second].piece->color() != color)
				result.insert(searchPos);
			break;
		}
		result.insert(searchPos);
		searchPos.first = searchPos.first + 1;
		searchPos.second = searchPos.second + 1;
	}
	searchPos = { start.first - 1, start.second + 1 };
	while (searchPos != start)
	{
		if (board[searchPos.first][searchPos.second].piece)
		{
			if (board[searchPos.first][searchPos.second].piece->color() != color)
				result.insert(searchPos);
			break;
		}
		result.insert(searchPos);
		searchPos.first = searchPos.first - 1;
		searchPos.second = searchPos.second + 1;
	}
	searchPos = { start.first + 1, start.second - 1 };
	while (searchPos != start)
	{
		if (board[searchPos.first][searchPos.second].piece)
		{
			if (board[searchPos.first][searchPos.second].piece->color() != color)
				result.insert(searchPos);
			break;
		}
		result.insert(searchPos);
		searchPos.first = searchPos.first + 1;
		searchPos.second = searchPos.second - 1;
	}
	searchPos = { start.first - 1, start.second - 1 };
	while (searchPos != start)
	{
		if (board[searchPos.first][searchPos.second].piece)
		{
			if (board[searchPos.first][searchPos.second].piece->color() != color)
				result.insert(searchPos);
			break;
		}
		result.insert(searchPos);
		searchPos.first = searchPos.first - 1;
		searchPos.second = searchPos.second - 1;
	}
	return result;
}


std::set<BoardPos> possibleRooklikeMoves(const Piece& p)
{
	const BoardPos& pos = p.pos();
	const BoardType& board = p.board();
	const PieceColor& color = p.color();
	std::set<BoardPos> result;
	// moving left
	for (BoardX x = pos.first - 1; x != pos.first; x = x - 1)
	{
		if (board[x][pos.second].piece)
		{
			if (board[x][pos.second].piece->color() != color)
				result.insert({ x, pos.second });
			break;
		}
		result.insert({ x, pos.second });
	}
	// moving right
	for (BoardX x = pos.first + 1; x != pos.first; x = x + 1)
	{
		if (board[x][pos.second].piece)
		{
			if (board[x][pos.second].piece->color() != color)
				result.insert({ x, pos.second });
			break;
		}
		result.insert({ x, pos.second });
	}


	// moving down
	for (BoardY y = pos.second - 1; y != pos.second; y = y - 1)
	{
		if (board[pos.first][y].piece)
		{
			if (board[pos.first][y].piece->color() != color)
				result.insert({ pos.first, y });
			break;
		}
		result.insert({ pos.first, y });
	}
	// moving up
	for (BoardY y = pos.second + 1; y != pos.second; y = y + 1)
	{
		if (board[pos.first][y].piece)
		{
			if (board[pos.first][y].piece->color() != color)
				result.insert({ pos.first, y });
			break;
		}
		result.insert({ pos.first, y });
	}


	return result;
}

std::shared_ptr<Piece> findKing(BoardType& board, PieceColor color)
{
	for (auto& col : board)
	{
		for (auto& cell : col)
		{
			if (cell.piece && cell.piece->pieceType() == PieceType::eKing && 
				cell.piece->color() == color)
				return cell.piece;
		}
	}
	return nullptr;
}