#define OLC_PGE_APPLICATION

#include <iostream>
#include <stack>

#include "olcPixelGameEngine.h"

using namespace std;

class Engine : public olc::PixelGameEngine
{
public:
	Engine()
	{
		sAppName = "Maze Generation";
	}

private:
	int mazeWidth;
	int mazeHeight;
	int* maze;

	enum
	{
		CELL_PATH_N = 0x01,
		CELL_PATH_E = 0x02,
		CELL_PATH_S = 0x04,
		CELL_PATH_W = 0x08,
		CELL_VISITED = 0x10
	};

	// algorith variables
	int visitedCells;
	int pathWidth;
	stack<pair<int, int>> m_stack; // (x, y) coordinate pair

protected:
	bool OnUserCreate() override
	{
		// set maze parameters
		mazeWidth = 40;
		mazeHeight = 25;
		maze = new int[mazeWidth * mazeHeight];
		memset(maze, 0x00, mazeWidth * mazeHeight * sizeof(int));
		pathWidth = 3;

		// starting cell
		int x = 0;
		int y = 0;
		m_stack.push(make_pair(x, y));
		maze[y * mazeWidth + x] = CELL_VISITED;
		visitedCells = 1;

		srand(clock());

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		this_thread::sleep_for(5ms);

		auto offset = [&](int x, int y)
		{
			return (m_stack.top().second + y) * mazeWidth + (m_stack.top().first + x);
		};

		if (visitedCells < mazeWidth * mazeHeight)
		{
			vector<int> neighbours;

			// north
			if (m_stack.top().second > 0 && (maze[offset(0, -1)] & CELL_VISITED) == 0)
				neighbours.push_back(0);
			// east
			if (m_stack.top().first < mazeWidth - 1 && (maze[offset(1, 0)] & CELL_VISITED) == 0)
				neighbours.push_back(1);
			// west
			if (m_stack.top().first > 0 && (maze[offset(-1, 0)] & CELL_VISITED) == 0)
				neighbours.push_back(2);
			// south
			if (m_stack.top().second < mazeHeight - 1 && (maze[offset(0, 1)] & CELL_VISITED) == 0)
				neighbours.push_back(3);

			if (!neighbours.empty())
			{
				int nextCellDir = neighbours[rand() % neighbours.size()];

				switch (nextCellDir)
				{
					// north
				case 0:
					maze[offset(0, -1)] |= CELL_VISITED + CELL_PATH_S;
					maze[offset(0, 0)] |= CELL_PATH_N;
					m_stack.push(make_pair(m_stack.top().first + 0, m_stack.top().second + -1));
					break;

					// east
				case 1:
					maze[offset(1, 0)] |= CELL_VISITED + CELL_PATH_W;
					maze[offset(0, 0)] |= CELL_PATH_E;
					m_stack.push(make_pair(m_stack.top().first + 1, m_stack.top().second + 0));
					break;

					// west
				case 2:
					maze[offset(-1, 0)] |= CELL_VISITED + CELL_PATH_E;
					maze[offset(0, 0)] |= CELL_PATH_W;
					m_stack.push(make_pair(m_stack.top().first + -1, m_stack.top().second + 0));
					break;

					// south
				case 3:
					maze[offset(0, 1)] |= CELL_VISITED + CELL_PATH_N;
					maze[offset(0, 0)] |= CELL_PATH_S;
					m_stack.push(make_pair(m_stack.top().first + 0, m_stack.top().second + 1));
					break;
				}

				visitedCells++;
			}
			else
				m_stack.pop();
		}

		// drawing

		Clear(olc::WHITE);

		for (int x = 0; x < mazeWidth; x++)
		{
			for (int y = 0; y < mazeHeight; y++)
			{
				for (int py = 0; py < pathWidth; py++)
					for (int px = 0; px < pathWidth; px++)
					{
						if (maze[y * mazeWidth + x] & CELL_VISITED)
							Draw(x * (pathWidth + 1) + px, y * (pathWidth + 1) + py, olc::BLUE);
						else
							Draw(x * (pathWidth + 1) + px, y * (pathWidth + 1) + py, olc::YELLOW);
					}

				for (int p = 0; p < pathWidth; p++)
				{
					if (maze[y * mazeWidth + x] & CELL_PATH_S)
						Draw(x * (pathWidth + 1) + p, y * (pathWidth + 1) + pathWidth, olc::BLUE);

					if (maze[y * mazeWidth + x] & CELL_PATH_E)
						Draw(x * (pathWidth + 1) + pathWidth, y * (pathWidth + 1) + p, olc::BLUE);
				}
			}
		}

		for (int px = 0; px < pathWidth; px++)
			for (int py = 0; py < pathWidth; py++)
				Draw(m_stack.top().first * (pathWidth + 1) + px, m_stack.top().second * (pathWidth + 1) + py, olc::GREEN);

		return true;
	}
};

int main()
{
	Engine engine;

	if (engine.Construct(160, 100, 8, 8));
		engine.Start();

	return 0;
}