#ifndef CHESSWRAP_GAME_STATE_H
#define CHESSWRAP_GAME_STATE_H

#include <array>
#include <exception>
#include <memory>
#include <optional>
#include <set>
#include <stdexcept>

#include "SDL2/SDL.h"

#include "global.h"
#include "pieces.h"

class ChessWrapGame
{
public:

	ChessWrapGame();
	~ChessWrapGame();

	void run();

	void update();
	void render();

private:

	enum class WinState
	{
		eNone,
		eCheckmate,
		eStalemate
	};

	//void moveSelectedPiece(const BoardPos& newPos);

	void fillDefaultCellColors();
	void setupDefaultBoard();
	void highlightPossibleMoves(Piece& piece);
	std::shared_ptr<Piece>& selectedPiece();

	void fillCells();
	void drawGrid();
	void drawStatusInfo();
	void drawResetButton();
	void drawResetTiming();

	void onClick(SDL_MouseButtonEvent& clickEvent);
	void onReleaseClick(SDL_MouseButtonEvent& releaseEvent);
	bool isOnResetButton(int x, int y);

	WinState checkForCheckmate();
	void finishMove();
	void moveRookIfCastling(const BoardPos& src, const BoardPos& dst);
	bool isEnPassantMove(const BoardPos& src, const BoardPos& dst);
	bool tryPromotePieceAt(const BoardPos& pos);

	void loadTurnText();
	SDL_Texture* turnText();
	void loadWinText();
	SDL_Texture* winText();
	void loadResetText();
	SDL_Texture* resetText();

	template <typename PieceType>
	std::enable_if_t<std::is_base_of_v<Piece, PieceType>, void>
		placePieceAt(BoardPos pos, PieceColor color)
	{
		m_board[pos.first][pos.second].piece = std::make_shared<PieceType>(m_board, color, pos, m_turnCount);
	}

	template <typename PieceType>
	std::enable_if_t<std::is_base_of_v<Piece, PieceType>, void>
		placeStartingPieceAt(BoardPos pos, PieceColor color)
	{
		if (color == PieceColor::eWhite)
		{
			BoardPos dst = { pos.first + board_x_margin, pos.second + board_y_margin };
			m_board[dst.first][dst.second].piece = 
				std::make_shared<PieceType>(m_board, color, dst, m_turnCount);
		}
		else
		{
			BoardPos dst = { pos.first + board_x_margin, board_h - 1 - (7 - pos.second + board_y_margin) };
			m_board[dst.first][dst.second].piece =
				std::make_shared<PieceType>(m_board, color, dst, m_turnCount);
		}
	}

	static inline constexpr ColorRGB white_cell_color = { UINT8_MAX, UINT8_MAX, UINT8_MAX };
	static inline constexpr ColorRGB black_cell_color = { 0, 0, 0 };
	static inline constexpr ColorRGB piece_selected_color = { 100, 100, 200 };
	static inline constexpr ColorRGB possible_move_color = { UINT8_MAX, UINT8_MAX, 200 };
	static inline constexpr ColorRGB reset_button_color = { 3 * (UINT8_MAX / 4), 3 * (UINT8_MAX / 4), 3 * (UINT8_MAX / 4) };
	static inline constexpr ColorRGB reset_pressed_color = { 5 * (UINT8_MAX / 8), 5 * (UINT8_MAX / 8), 5 * (UINT8_MAX / 8) };

	static inline constexpr int turn_text_w = sidebar_w;
	static inline constexpr int turn_text_h = 50;
	static inline constexpr int win_text_w = sidebar_w;
	static inline constexpr int win_text_h = 50;
	static inline constexpr int reset_button_w = sidebar_w;
	static inline constexpr int reset_button_h = 150;
	static inline constexpr int reset_button_x = board_px_w;
	static inline constexpr int reset_button_y = board_px_h - reset_button_h;
	static inline constexpr int reset_text_w = 100;
	static inline constexpr int reset_text_h = 50;
	static inline constexpr int reset_text_x = board_px_w + ((reset_button_w - reset_text_w) / 2);
	static inline constexpr int reset_text_y = board_px_h - reset_button_h + ((reset_button_h - reset_text_h) / 2);

	static inline constexpr uint32_t reset_unpress_delay_ms = 150;
	static inline constexpr uint32_t reset_hold_time_ms = 2000;

	bool m_quit = false;
	SDL_Window* m_window;
	SDL_Renderer* m_renderer;
	SDL_Texture* m_turnb = nullptr;
	SDL_Texture* m_turnw = nullptr;
	SDL_Texture* m_winb = nullptr;
	SDL_Texture* m_winw = nullptr;
	SDL_Texture* m_stale = nullptr;
	SDL_Texture* m_resetText = nullptr;
	bool m_resetPressed = false;
	uint32_t m_resetPressTime = 0;
	std::optional<BoardPos> m_selectedPos;
	WinState m_winState = WinState::eNone;
	unsigned int m_turnCount = 0;

	BoardType m_board;
	BoardPos m_kingWPos, m_kingBPos;
	PieceColor m_activePlayer = PieceColor::eWhite;
};

#endif // CHESSWRAP_GAME_STATE_H