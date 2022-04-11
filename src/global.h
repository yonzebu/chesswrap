#ifndef CHESSWRAP_GLOBAL_H
#define CHESSWRAP_GLOBAL_H

#include <array>
#include <exception>
#include <memory>
#include <set>
#include <stdexcept>
#include <tuple>

#include "SDL2/SDL_surface.h"

template <int ModVal>
class IntModN
{
public:
	static constexpr inline int N = ModVal;

	constexpr IntModN(int x = {}) noexcept
		: m_val{ restrictToModN(x) }
	{}
	constexpr IntModN(unsigned int x) noexcept
		: IntModN(static_cast<int>(x))
	{}

	template <int OtherN>
	constexpr IntModN(IntModN<OtherN> x) noexcept
		: IntModN(x.m_val)
	{}

	template <int OtherN>
	friend class IntModN;

	//template <int OtherN>
	//friend IntModN<OtherN>::IntModN(IntModN x);

	//constexpr IntModN operator+() const noexcept { return m_val; }
	//constexpr IntModN operator-() const noexcept { return -m_val; }

	/*constexpr IntModN operator+(IntModN rhs) const noexcept
	{
		return get() + rhs.get();
	}
	constexpr IntModN operator-(IntModN rhs) const noexcept
	{
		return get() - rhs.get();
	}
	constexpr IntModN operator*(IntModN rhs) const noexcept
	{
		return get() * rhs.get();
	}
	constexpr IntModN operator/(IntModN rhs) const noexcept
	{
		return get() / rhs.get();
	}*/

	/*constexpr bool operator==(IntModN rhs) const noexcept
	{
		return get() == rhs.get();
	}
	constexpr bool operator!=(IntModN rhs) const noexcept
	{
		return !operator==(rhs);
	}
	constexpr bool operator<(IntModN rhs) const noexcept
	{
		return get() < rhs.get();
	}
	constexpr bool operator>(IntModN rhs) const noexcept
	{
		return get() > rhs.get();
	}
	constexpr bool operator<=(IntModN<N> rhs) noexcept
	{
		return !operator>(rhs);
	}
	constexpr bool operator>=(IntModN rhs) const noexcept
	{
		return !operator<(rhs);
	}*/

	constexpr IntModN& operator+=(IntModN rhs) const noexcept
	{
		m_val = restrictToModN(operator+(rhs));
		return *this;
	}
	constexpr IntModN& operator-=(IntModN rhs) const noexcept
	{
		m_val = restrictToModN(operator-(rhs));
		return *this;
	}
	constexpr IntModN& operator*=(IntModN rhs) const noexcept
	{
		m_val = restrictToModN(operator*(rhs));
		return *this;
	}
	constexpr IntModN& operator/=(IntModN rhs) const noexcept
	{
		m_val = restrictToModN(operator/(rhs));
		return *this;
	}
	constexpr IntModN& operator%=(IntModN rhs) const noexcept
	{
		m_val = restrictToModN(operator%(rhs));
		return *this;
	}

	constexpr unsigned int get() const noexcept
	{
		return static_cast<unsigned int>(restrictToModN(m_val));
	}

	/*explicit constexpr operator unsigned int() const noexcept
	{
		return get();
	}*/

	constexpr operator unsigned int() const noexcept
	{
		return get();
	}

private:
	constexpr int restrictToModN(int x) const noexcept 
	{ 
		return (x % N) >= 0 ? x % N : (x % N) + N;
	}
	int m_val;
};

//template <int N>
//constexpr IntModN<N> operator+(int lhs, IntModN<N> rhs) noexcept
//{
//	return rhs + lhs;
//}
//template <int N>
//constexpr IntModN<N> operator-(int lhs, IntModN<N> rhs) noexcept
//{
//	return (-rhs) + lhs;
//}
//template <int N>
//constexpr IntModN<N> operator*(int lhs, IntModN<N> rhs) noexcept
//{
//	return rhs * lhs;
//}
//template <int N>
//constexpr IntModN<N> operator/(int lhs, IntModN<N> rhs) noexcept
//{
//	return lhs / rhs.get();
//}
//
//template <int N>
//constexpr bool operator==(int lhs, IntModN<N> rhs) noexcept
//{
//	return rhs == lhs;
//}
//
//template <int N>
//constexpr bool operator!=(int lhs, IntModN<N> rhs) noexcept
//{
//	return rhs != lhs;
//}
//
//template <int N>
//constexpr bool operator<(int lhs, IntModN<N> rhs) noexcept
//{
//	return rhs > lhs;
//}
//
//template <int N>
//constexpr bool operator>(int lhs, IntModN<N> rhs) noexcept
//{
//	return rhs < lhs;
//}
//
//template <int N>
//constexpr bool operator<=(int lhs, IntModN<N> rhs) noexcept
//{
//	return rhs >= lhs;
//}
//
//template <int N>
//constexpr bool operator>=(int lhs, IntModN<N> rhs) noexcept
//{
//	return rhs <= lhs;
//}

template <int Lower, int Upper>
class BoundedInt
{
public:
	static constexpr inline int LowerBound = Lower;
	static constexpr inline int UpperBound = Upper;

	constexpr BoundedInt(int x)
		: m_val{ boundInt(x) }
	{}

	constexpr int get() const noexcept
	{
		return boundInt(m_val);
	}

	constexpr operator int() const noexcept
	{
		return get();
	}

	static constexpr int boundInt(int x) noexcept
	{
		return std::min(std::max(x, Lower), Upper);
	}

private:
	int m_val;
};

// configurable settings
inline constexpr bool fill_margins_x = false;
inline constexpr bool fill_margins_y = true;
inline constexpr int board_w = 8;
inline constexpr int board_h = 10;
inline constexpr int board_y_margin = 1;
inline constexpr int board_x_margin = 0;
inline constexpr int window_w = 800;
inline constexpr int board_px_w = 600;
// automatically calculated from tweaked settings
inline constexpr int sidebar_w = window_w - board_px_w;
inline constexpr int window_h = (board_px_w * board_h) / board_w;
inline constexpr int board_px_h = window_h;
inline constexpr int cell_w = board_px_w / board_w;
inline constexpr int cell_h = board_px_h / board_h;

// need to fit basic pieces
static_assert(board_w - 2 * board_x_margin >= 8);
// need to fit basic rows + at least margin of 1
static_assert(board_h - 2 * board_y_margin >= 4);
static_assert(board_y_margin > 0);

//template <int W, int H>
//using Position = std::pair<IntModN<W>, IntModN<H>>;
using BoardX = IntModN<board_w>;
using BoardY = IntModN<board_h>;
using BoardPos = std::pair<BoardX, BoardY>;

//template <int LowerX, int UpperX, int LowerY, int UpperY>
//using Position = std::pair<BoundedInt<LowerX, UpperX, LowerX, UpperY>>;
//using BoardX = BoundedInt<0, board_w-1>;
//using BoardY = BoundedInt<0, board_h-1>;
//using BoardPos = std::pair<BoardX, BoardY>;


using ColorRGB = std::array<uint8_t, 3>;

struct Piece;
struct CellState
{
	// this is so the board can be copied while retaining known lifetimes AND 
	// runtime polymorphism
	// it's not ideal but chess is not a high performance game
	std::shared_ptr<Piece> piece;
	ColorRGB color;
};

using BoardType = std::array<std::array<CellState, board_h>, board_w>;

BoardX windowToBoardX(int x);
BoardY windowToBoardY(int x);
BoardPos windowToBoardCoords(int windowX, int windowY);
int boardToWindowX(int x);
int boardToWindowY(int x);
std::pair<int, int> boardToWindowCoords(const BoardPos& pos);
bool isOnBoard(int screenX, int screenY);

inline constexpr ColorRGB averageColor(
	const ColorRGB& color1, const ColorRGB& color2) noexcept
{
	return ColorRGB{ static_cast<uint8_t>((color1[0] + color2[0]) / 2),
		static_cast<uint8_t>((color1[1] + color2[1]) / 2),
		static_cast<uint8_t>((color1[2] + color2[2]) / 2) };
}

std::set<BoardPos> possibleBishoplikeMoves(const Piece&);
std::set<BoardPos> possibleRooklikeMoves(const Piece&);

enum class PieceColor;
std::shared_ptr<Piece> findKing(BoardType& board, PieceColor color);

class TextureNotLoaded : std::runtime_error
{
public:
	TextureNotLoaded()
		: std::runtime_error("failed to load a texture! did you move anything in the resources directory, perhaps?")
	{}
};

// doesn't work, don't use
//void invertSurfaceColors(SDL_Surface* surf);

#endif // CHESSWRAP_GLOBAL_H