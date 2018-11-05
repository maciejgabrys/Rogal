/*
	Maciej Gabryś
	gm41357, gr. 211a

	05.11.2018
*/

#include <ncurses.h>
#include <iostream>
#include <string>
#include "Constants.h"
#include "Exceptions.h"
#include "Map.h"
#include "Player.h"

void init() noexcept;
bool level_loader();
bool play_level(std::string path, Player* player) noexcept;
void quit() noexcept;

int main()
{
	init();
	char ch;
	bool loop = true;
	do
	{
		erase();
		printw("%s\n%s", menulogo, menutext);
		ch = getch();
		switch (ch)
		{
		case 'P':
		case 'p':
			try
			{
				if (level_loader() == true)
				{
					erase();
					printw("%s\n%s", menulogo, menuwin);
					getch();
				}
			}
			catch (Exception& e)
			{
				erase();
				printw(messagebadlevellist, e.what());
				getch();
			}
			break;
		case 'Q':
		case 'q':
			loop = false;
			break;
		}
	} while (loop == true);
	quit();
	return 0;
}

void init() noexcept
{
	initscr();
	raw();
	keypad(stdscr, true);
	noecho();
	start_color();
	use_default_colors();
	curs_set(0);
	init_pair(color_wall, COLOR_BLUE, -1);
	init_pair(color_floor, COLOR_WHITE, -1);
	init_pair(color_hallway, COLOR_CYAN, -1);
	init_pair(color_coin, COLOR_YELLOW, -1);
	init_pair(color_player, COLOR_RED, -1);
}

bool level_loader()
{
	Player* player = new Player{};
	std::ifstream list;
	std::string path;
	bool empty = true;
	bool completed = true;
	list.open("List.txt");
	if (list.fail())
	{
		list.close();
		delete player;
		throw ExceptionBadLevelList{};
	}
	else
	{
		while (list >> path)
		{
			empty = false;
			if (play_level(path, player) == false)
			{
				completed = false;
				break;
			}
		}
		list.close();
		if (empty == true)
		{
			throw ExceptionEmptyLevelList{};
		}
	}
	delete player;
	return completed;
}

bool play_level(std::string path, Player* player) noexcept
{
	Map* map;
	try
	{
		map = new Map{ path };
	}
	catch (Exception& e)
	{
		erase();
		printw(messagebadmap, path.c_str(), e.what());
		getch();
		return false;
	}
	player->teleport(map);
	while (map->remaining_coins() > 0)
	{
		erase();
		printw(statusbar, path.c_str(), map->remaining_coins(), player->get_amount());
		map->display();
		player->display();
		switch (getch())
		{
		case 'A':
		case 'a':
			player->walk(map, direction_left);
			break;
		case 'D':
		case 'd':
			player->walk(map, direction_right);
			break;
		case 'W':
		case 'w':
			player->walk(map, direction_up);
			break;
		case 'S':
		case 's':
			player->walk(map, direction_down);
			break;
		case 'E':
		case 'e':
			delete map;
			return false;
		}
	}
	delete map;
	return true;
}

void quit() noexcept
{
	endwin();
}
