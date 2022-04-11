#include "game_state.h"

#include <algorithm>
#include <iostream>
#include <vector>

#include "SDL2/SDL_events.h"
#include "SDL2/SDL_video.h"

#include "global.h"
#include "pieces.h"

ChessWrapGame::ChessWrapGame()
	: m_window{ nullptr }, m_board{}, m_selectedPos{}
{
	m_window = SDL_CreateWindow("ChessWrap",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		window_w, window_h, 0);
	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

}

ChessWrapGame::~ChessWrapGame()
{
	if (Piece::whiteTex<Pawn>)
		SDL_DestroyTexture(Piece::whiteTex<Pawn>);
	if (Piece::blackTex<Pawn>)
		SDL_DestroyTexture(Piece::blackTex<Pawn>);
	if (Piece::whiteTex<Rook>)
		SDL_DestroyTexture(Piece::whiteTex<Rook>);
	if (Piece::blackTex<Rook>)
		SDL_DestroyTexture(Piece::blackTex<Rook>);
	if (Piece::whiteTex<Bishop>)
		SDL_DestroyTexture(Piece::whiteTex<Bishop>);
	if (Piece::blackTex<Bishop>)
		SDL_DestroyTexture(Piece::blackTex<Bishop>);
	if (Piece::whiteTex<Knight>)
		SDL_DestroyTexture(Piece::whiteTex<Knight>);
	if (Piece::blackTex<Knight>)
		SDL_DestroyTexture(Piece::blackTex<Knight>);
	if (Piece::whiteTex<Queen>)
		SDL_DestroyTexture(Piece::whiteTex<Queen>);
	if (Piece::blackTex<Queen>)
		SDL_DestroyTexture(Piece::blackTex<Queen>);
	if (Piece::whiteTex<King>)
		SDL_DestroyTexture(Piece::whiteTex<King>);
	if (Piece::blackTex<King>)
		SDL_DestroyTexture(Piece::blackTex<King>);
	if (m_turnb)
		SDL_DestroyTexture(m_turnb);
	if (m_turnw)
		SDL_DestroyTexture(m_turnw);
	if (m_winb)
		SDL_DestroyTexture(m_winb);
	if (m_winw)
		SDL_DestroyTexture(m_winw);
	if (m_stale)
		SDL_DestroyTexture(m_stale);
	if (m_resetText)
		SDL_DestroyTexture(m_resetText);
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
}

void ChessWrapGame::run()
{
	m_quit = !m_window;
	setupDefaultBoard();
	while (!m_quit)
	{
		update();
		render();
	}
}

void ChessWrapGame::update()
{
	SDL_Event currentEvent;
	//highlightHoveredSquare();
	while (SDL_PollEvent(&currentEvent))
	{
		if (currentEvent.type == SDL_MOUSEBUTTONDOWN)
		{
			onClick(currentEvent.button);
		}
		else if (currentEvent.type == SDL_MOUSEBUTTONUP)
		{
			onReleaseClick(currentEvent.button);
		}
		else if (currentEvent.type == SDL_WINDOWEVENT)
		{
			if (currentEvent.window.windowID == SDL_WINDOWEVENT_CLOSE)
				m_quit = true;
		}
		else if (currentEvent.type == SDL_QUIT)
			m_quit = true;
	}
	if (m_resetPressed && SDL_TICKS_PASSED(SDL_GetTicks(), m_resetPressTime + reset_hold_time_ms))
	{
		m_resetPressed = false;
		setupDefaultBoard();
	}
}

void ChessWrapGame::render()
{
	SDL_SetRenderDrawColor(m_renderer, UINT8_MAX/2, UINT8_MAX/2, UINT8_MAX/2, UINT8_MAX);
	SDL_RenderClear(m_renderer);

	fillCells();
	drawGrid();
	drawStatusInfo();

	SDL_RenderPresent(m_renderer);
}

void ChessWrapGame::highlightPossibleMoves(Piece& piece)
{
	auto moves = piece.possibleMoves();
	for (auto& pos : moves)
	{
		auto& cell = m_board[pos.first.get()][pos.second.get()];
		if (cell.piece && cell.piece->pieceType() == PieceType::eKing)
		{
			// skip this cell if the king would be in check
		}
		cell.color = averageColor(possible_move_color, cell.color);
	}
}

void ChessWrapGame::fillDefaultCellColors()
{
	for (int x = 0; x < board_w; ++x)
	{
		for (int y = 0; y < board_h; ++y)
		{
			if ((x + 1 + (y % 2)) % 2 == 0)
				m_board[x][y].color = black_cell_color;
			else
				m_board[x][y].color = white_cell_color;
		}
	}
}

void ChessWrapGame::setupDefaultBoard()
{
	m_activePlayer = PieceColor::eWhite;
	m_selectedPos.reset();
	m_winState = WinState::eNone;
	for (auto& col : m_board)
		for (auto& cell : col)
			cell.piece.reset();
	fillDefaultCellColors();

	if (fill_margins_y)
	{
		for (int i = 0; i < board_y_margin; ++i)
		{
			for (int x = 0; x < board_w; ++x)
			{
				placePieceAt<Pawn>({ x, i }, PieceColor::eWhite);
			}
		}
	}
	if (fill_margins_x)
	{
		for (int y = 0; y < 2; ++y)
		{
			for (int x = 0; x < board_x_margin; ++x)
			{
				placePieceAt<Pawn>({ x, y + board_y_margin }, PieceColor::eWhite);
				placePieceAt<Pawn>({ board_w - 1 - x, y + board_y_margin }, PieceColor::eWhite);
			}
		}
	}

	placeStartingPieceAt<Pawn>({ 0, 1 }, PieceColor::eWhite);
	placeStartingPieceAt<Pawn>({ 1, 1 }, PieceColor::eWhite);
	placeStartingPieceAt<Pawn>({ 2, 1 }, PieceColor::eWhite);
	placeStartingPieceAt<Pawn>({ 3, 1 }, PieceColor::eWhite);
	placeStartingPieceAt<Pawn>({ 4, 1 }, PieceColor::eWhite);
	placeStartingPieceAt<Pawn>({ 5, 1 }, PieceColor::eWhite);
	placeStartingPieceAt<Pawn>({ 6, 1 }, PieceColor::eWhite);
	placeStartingPieceAt<Pawn>({ 7, 1 }, PieceColor::eWhite);
	placeStartingPieceAt<Rook>({ 0, 0 }, PieceColor::eWhite);
	placeStartingPieceAt<Rook>({ 7, 0 }, PieceColor::eWhite);
	placeStartingPieceAt<Knight>({ 1, 0 }, PieceColor::eWhite);
	placeStartingPieceAt<Knight>({ 6, 0 }, PieceColor::eWhite);
	placeStartingPieceAt<Bishop>({ 2, 0 }, PieceColor::eWhite);
	placeStartingPieceAt<Bishop>({ 5, 0 }, PieceColor::eWhite);
	placeStartingPieceAt<Queen>({ 3, 0 }, PieceColor::eWhite);
	placeStartingPieceAt<King>({ 4, 0 }, PieceColor::eWhite);
	m_kingWPos = { 4, 1 };

	if (fill_margins_y)
	{
		for (int y = board_h - 1 - board_y_margin; y < board_h; ++y)
		{
			for (int x = 0; x < board_w; ++x)
			{
				placePieceAt<Pawn>({ x, y }, PieceColor::eBlack);
			}
		}
	}
	if (fill_margins_x)
	{
		for (int y = 0; y < 2; ++y)
		{
			for (int x = 0; x < board_x_margin; ++x)
			{
				placePieceAt<Pawn>({ x, board_h - 1 - board_y_margin - y }, PieceColor::eBlack);
				placePieceAt<Pawn>({ board_w - 1 - x, board_h - 1 - board_y_margin - y }, PieceColor::eBlack);
			}
		}
	}
	
	placeStartingPieceAt<Pawn>({ 0, 6 }, PieceColor::eBlack);
	placeStartingPieceAt<Pawn>({ 1, 6 }, PieceColor::eBlack);
	placeStartingPieceAt<Pawn>({ 2, 6 }, PieceColor::eBlack);
	placeStartingPieceAt<Pawn>({ 3, 6 }, PieceColor::eBlack);
	placeStartingPieceAt<Pawn>({ 4, 6 }, PieceColor::eBlack);
	placeStartingPieceAt<Pawn>({ 5, 6 }, PieceColor::eBlack);
	placeStartingPieceAt<Pawn>({ 6, 6 }, PieceColor::eBlack);
	placeStartingPieceAt<Pawn>({ 7, 6 }, PieceColor::eBlack);
	placeStartingPieceAt<Rook>({ 0, 7 }, PieceColor::eBlack);
	placeStartingPieceAt<Rook>({ 7, 7 }, PieceColor::eBlack);
	placeStartingPieceAt<Knight>({ 1, 7 }, PieceColor::eBlack);
	placeStartingPieceAt<Knight>({ 6, 7 }, PieceColor::eBlack);
	placeStartingPieceAt<Bishop>({ 2, 7 }, PieceColor::eBlack);
	placeStartingPieceAt<Bishop>({ 5, 7 }, PieceColor::eBlack);
	placeStartingPieceAt<Queen>({ 3, 7 }, PieceColor::eBlack);
	placeStartingPieceAt<King>({ 4, 7 }, PieceColor::eBlack);
	m_kingBPos = { 4, 8 };
}

std::shared_ptr<Piece>& ChessWrapGame::selectedPiece()
{
	auto selectedX = static_cast<unsigned int>(m_selectedPos->first);
	auto selectedY = static_cast<unsigned int>(m_selectedPos->second);
	return m_board[selectedX][selectedY].piece;
}

void ChessWrapGame::fillCells()
{
	for (int x = 0; x < board_w; ++x)
	{
		for (int y = 0; y < board_h; ++y)
		{
			SDL_SetRenderDrawColor(m_renderer, m_board[x][y].color[0], 
				m_board[x][y].color[1], m_board[x][y].color[2], UINT8_MAX);
			SDL_Rect cellRect{ boardToWindowX(x), boardToWindowY(y) - cell_h, cell_w, cell_h };
			SDL_RenderFillRect(m_renderer, &cellRect);

			if (m_board[x][y].piece)
				m_board[x][y].piece->render(m_renderer);
		}
	}
}

void ChessWrapGame::drawGrid()
{
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, UINT8_MAX);
	for (int i = 1; i < board_w; ++i)
		SDL_RenderDrawLine(m_renderer, boardToWindowX(i), 0, boardToWindowX(i), board_px_h);
	for (int i = 1; i < board_h; ++i)
		SDL_RenderDrawLine(m_renderer, 0, boardToWindowY(i), board_px_w, boardToWindowY(i));
}

void ChessWrapGame::onClick(SDL_MouseButtonEvent& clickEvent)
{
	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);
	if (isOnBoard(mouseX, mouseY))
	{
		auto&& [cellX, cellY] = windowToBoardCoords(mouseX, mouseY);
		if (m_selectedPos)
		{
			auto& [selectedX, selectedY] = m_selectedPos.value();
			// deselect if it's the selected square
			if (selectedX == cellX && selectedY == cellY)
			{
				fillDefaultCellColors();
				m_selectedPos.reset();
			}
			// make the move if it's a move!
			else
			{
				auto&& attackMoves = selectedPiece()->attackMoves();
				auto&& movementMoves = selectedPiece()->movementMoves();
				auto&& possibleMoves = selectedPiece()->possibleMoves();
				BoardPos toFind = BoardPos{ cellX, cellY };
				if (possibleMoves.end() !=
					std::find(possibleMoves.begin(), possibleMoves.end(),
						toFind))
				{
					// castles if this is a castling move
					moveRookIfCastling({ selectedX, selectedY }, toFind);

					if (isEnPassantMove({ selectedX, selectedY }, { cellX, cellY }))
					{
						int sign = selectedPiece()->color() == PieceColor::eWhite ? -1 : 1;
						m_board[cellX][BoardY{ cellY + sign }].piece.reset();
					}

					// move the actual moved piece
					m_board[cellX.get()][cellY.get()].piece = std::move(selectedPiece());
					m_board[cellX.get()][cellY.get()].piece->setPos({ cellX, cellY });
					
					tryPromotePieceAt({ cellX, cellY });
					finishMove();
				}
			}
		}
		else
		{
			m_selectedPos = { cellX, cellY };
			auto& pos = m_selectedPos.value();
			if (selectedPiece() && selectedPiece()->color() == m_activePlayer)
			{
				highlightPossibleMoves(*selectedPiece());
				m_board[cellX.get()][cellY.get()].color = averageColor(piece_selected_color, m_board[cellX.get()][cellY.get()].color);
			}
			else
				m_selectedPos.reset();
		}

		//std::cout << "click at: " << cellX.get() << ", " << cellY.get() << std::endl;
	}
	else
	{
		if (isOnResetButton(mouseX, mouseY))
		{
			m_resetPressed = true;
			m_resetPressTime = SDL_GetTicks();
		}
	}
}

void ChessWrapGame::onReleaseClick(SDL_MouseButtonEvent& releaseEvent)
{
	if (m_resetPressed)
	{
		if (SDL_TICKS_PASSED(SDL_GetTicks(), m_resetPressTime + reset_hold_time_ms))
		{
			setupDefaultBoard();
		}
	}
	m_resetPressed = false;
}

bool ChessWrapGame::isOnResetButton(int x, int y)
{
	return x > board_px_w && x < window_w && 
		y < window_h && y > window_h - reset_button_h;
}

auto ChessWrapGame::checkForCheckmate() -> WinState
{
	//return WinState::eNone;

	auto kingPtr = findKing(m_board, m_activePlayer);
	bool canAnyMove = false;
	for (auto& col : m_board)
	{
		for (auto& cell : col)
		{
			if (cell.piece && cell.piece->color() == m_activePlayer)
			{
				if (cell.piece->possibleMoves().size() > 0)
					canAnyMove = true;
			}
		}
	}

	if (canAnyMove)
		return WinState::eNone;
	if (King::wouldBeChecked(*kingPtr))
		return WinState::eCheckmate;
	else
		return WinState::eStalemate;
}

void ChessWrapGame::moveRookIfCastling(const BoardPos& src, const BoardPos& dst)
{
	if (!m_board[src.first][src.second].piece)
		return;
	auto& piecePtr = m_board[src.first][src.second].piece;
	// check for a castling move (left)
	if (King::canCastle(*piecePtr, King::CastleDirection::eLeft) &&
		dst.first == src.first - 2 && src.second == dst.second)
	{
		// move the rook, set its pos and then move it
		m_board[0][src.second].piece->setPos({ dst.first + 1, src.second });
		m_board[dst.first + 1][src.second].piece = std::move(m_board[0][src.second].piece);
	}
	// check for castling move (right)
	if (King::canCastle(*selectedPiece(), King::CastleDirection::eRight) &&
		dst.first == src.first + 2 && src.second == dst.second)
	{
		m_board[board_w - 1][src.second].piece->setPos({ dst.first - 1, src.second });
		m_board[dst.first - 1][src.second].piece = std::move(m_board[board_w - 1][src.second].piece);
	}
}

void ChessWrapGame::finishMove()
{
	fillDefaultCellColors();
	m_selectedPos.reset();
	m_activePlayer = opposingColor(m_activePlayer);
	m_winState = checkForCheckmate();
	++m_turnCount;
}

bool ChessWrapGame::isEnPassantMove(const BoardPos& src, const BoardPos& dst)
{
	if (m_board[src.first][src.second].piece &&
		!m_board[dst.first][dst.second].piece &&
		m_board[src.first][src.second].piece->pieceType() == PieceType::ePawn)
	{
		auto& piecePtr = m_board[src.first][src.second].piece;
		auto color = piecePtr->color();
		auto sign = color == PieceColor::eWhite ? -1 : 1;
		if (m_board[dst.first][BoardY{ dst.second + sign }].piece)
		{
			auto& otherPtr = m_board[dst.first][BoardY{ dst.second + sign }].piece;
			if (otherPtr->color() == opposingColor(color) &&
				otherPtr->pieceType() == PieceType::ePawn &&
				otherPtr->lastPos() == BoardPos{ dst.first, dst.second - sign })
				return true;
		}
	}
	return false;
}

bool ChessWrapGame::tryPromotePieceAt(const BoardPos& pos)
{
	if (m_board[pos.first][pos.second].piece->pieceType() == PieceType::ePawn)
	{
		if (m_board[pos.first][pos.second].piece->color() == PieceColor::eWhite)
		{
			if (pos.second == board_h - 1)
				placePieceAt<Queen>({ pos.first, pos.second }, PieceColor::eWhite);
		}
		else
		{
			if (pos.second == 0)
				placePieceAt<Queen>({ pos.first, pos.second }, PieceColor::eBlack);
		}
	}
	return m_board[pos.first][pos.second].piece->pieceType() == PieceType::eQueen;
}

void ChessWrapGame::loadTurnText()
{
	auto surfb = SDL_LoadBMP("resources/turnb.bmp");
	auto surfw = SDL_LoadBMP("resources/turnw.bmp");
	if (!surfb || !surfw) throw TextureNotLoaded();
	m_turnb = SDL_CreateTextureFromSurface(m_renderer, surfb);
	m_turnw = SDL_CreateTextureFromSurface(m_renderer, surfw);
	SDL_FreeSurface(surfw);
	SDL_FreeSurface(surfb);
}

SDL_Texture* ChessWrapGame::turnText()
{
	return m_activePlayer == PieceColor::eWhite ? m_turnw : m_turnb;
}

void ChessWrapGame::loadWinText()
{
	auto surfcb = SDL_LoadBMP("resources/checkmateb.bmp");
	auto surfcw = SDL_LoadBMP("resources/checkmatew.bmp");
	auto surfStalemate = SDL_LoadBMP("resources/stalemate.bmp");
	if (!surfcb || !surfcw || !surfStalemate) throw TextureNotLoaded();
	m_winb = SDL_CreateTextureFromSurface(m_renderer, surfcb);
	m_winw = SDL_CreateTextureFromSurface(m_renderer, surfcw);
	m_stale = SDL_CreateTextureFromSurface(m_renderer, surfStalemate);
	SDL_FreeSurface(surfcw);
	SDL_FreeSurface(surfcb);
	SDL_FreeSurface(surfStalemate);
}

SDL_Texture* ChessWrapGame::winText()
{
	if (m_winState == WinState::eStalemate)
	{
		return m_stale;
	}
	else if (m_winState == WinState::eCheckmate)
	{
		return m_activePlayer == PieceColor::eWhite ? m_winw : m_winb;
	}
	return nullptr;
}

void ChessWrapGame::loadResetText()
{
	auto surf = SDL_LoadBMP("resources/reset.bmp");
	if (!surf) throw TextureNotLoaded();
	m_resetText = SDL_CreateTextureFromSurface(m_renderer, surf);
	SDL_FreeSurface(surf);
}

SDL_Texture* ChessWrapGame::resetText()
{
	return m_resetText;
}

void ChessWrapGame::drawStatusInfo()
{
	// turn text
	if (!turnText())
		loadTurnText();

	SDL_Rect turnTextRect{ board_px_w, 0, turn_text_w, turn_text_h };
	SDL_RenderCopy(m_renderer, turnText(), nullptr, &turnTextRect);

	if (m_winState != WinState::eNone)
	{
		if (!winText())
			loadWinText();

		SDL_Rect winTextRect{ board_px_w, window_h - reset_button_h - win_text_h,
			win_text_w, win_text_h };
		SDL_RenderCopy(m_renderer, winText(), nullptr, &winTextRect);
		// do win stuff here and THEN reset but i don't have energy right now
		//m_activePlayer = PieceColor::eWhite;
		//setupDefaultBoard();
	}

	drawResetButton();

}

void ChessWrapGame::drawResetButton()
{
	// button rect
	//if (m_resetPressed)
		SDL_SetRenderDrawColor(m_renderer, reset_button_color[0],
			reset_button_color[1], reset_button_color[2], UINT8_MAX);
	//else
		//SDL_SetRenderDrawColor(m_renderer, reset_pressed_color[0],
			//reset_pressed_color[1], reset_pressed_color[2], UINT8_MAX);
	SDL_Rect resetButtonRect{ reset_button_x, reset_button_y,
		reset_button_w, reset_button_h };
	SDL_RenderFillRect(m_renderer, &resetButtonRect);

	drawResetTiming();

	// reset text
	if (!resetText())
		loadResetText();
	SDL_Rect resetTextRect{ reset_text_x, reset_text_y,
		reset_text_w, reset_text_h };
	SDL_RenderCopy(m_renderer, resetText(), nullptr, &resetTextRect);
}

void ChessWrapGame::drawResetTiming()
{
	if (m_resetPressed)
	{
		double percentDone =
			static_cast<double>(SDL_GetTicks() - m_resetPressTime) /
			static_cast<double>(reset_hold_time_ms);
		percentDone = std::min(std::max(percentDone, 0.), 1.);

		double rect_w = percentDone * reset_button_w;

		SDL_Rect percentFillRect{ reset_button_x, reset_button_y,
			static_cast<int>(rect_w), reset_button_h };
		SDL_SetRenderDrawColor(m_renderer, reset_pressed_color[0],
			reset_pressed_color[1], reset_pressed_color[2], UINT8_MAX);
		SDL_RenderFillRect(m_renderer, &percentFillRect);
	}
}