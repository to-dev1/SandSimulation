#include <windows.h>
#include <vector>
#include <sstream>
#include <math.h>
#include <chrono>

bool running = true;

//Windows
int bufferWidth;
int bufferHeight;
BITMAPINFO bitmapInfo;

std::chrono::duration<float> TotalTime;
int TotalTimeCount;

//Cells
std::vector<int> Cells;
std::vector<int> ChangedCells;
std::vector<int> DeactiveCells;
std::vector<int> CellTypes;
std::vector<int> CellCounts;

int AllCellCounts[500];
int CellResources[500];
int CelltypesCount = 0;

int largestCellSize = 0;
int largestCellType = 0;

int Frames;
int repeatTotal = 0;

long long testCount;

float testCount2;

float DeltaTime = 0.0f;
float Update = 0.0f;
float RenderUpdate = 0.0f;
float EventUpdate = 0.0f;

bool EventType_1 = false;

int EventSpeed = 0;

int seed = 1343141;

int TotalCount = -1;

bool UpdateFirst = false;

LRESULT CALLBACK windowCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	switch (uMsg)
	{
	case WM_CLOSE:
	{
		running = false;

		std::ostringstream s;
		s << Frames;

		OutputDebugString("Frames: ");
		OutputDebugString(s.str().c_str());
		OutputDebugString("|");

		std::ostringstream s2;
		s2 << repeatTotal;

		OutputDebugString(s2.str().c_str());
		OutputDebugString("|");

		break;
	}
	case WM_DESTROY:
	{
		running = false;

		std::ostringstream s;
		s << Frames;

		OutputDebugString("Frames: ");
		OutputDebugString(s.str().c_str());
		OutputDebugString("|");

		std::ostringstream s2;
		s2 << repeatTotal;

		OutputDebugString(s2.str().c_str());
		OutputDebugString("|");

		break;
	}

	case WM_SIZE:
	{
		RECT rect;
		GetClientRect(hwnd, &rect);
		bufferWidth = rect.right - rect.left;
		bufferHeight = rect.bottom - rect.top;

		int bufferSize = bufferWidth * bufferHeight * sizeof(unsigned int);

		bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
		bitmapInfo.bmiHeader.biWidth = bufferWidth;
		bitmapInfo.bmiHeader.biHeight = bufferHeight;
		bitmapInfo.bmiHeader.biPlanes = 1;
		bitmapInfo.bmiHeader.biBitCount = 32;
		bitmapInfo.bmiHeader.biCompression = BI_RGB;

		break;
	}

	default:
	{
		result = DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	}

	return result;
}

inline int Random()
{
	seed = (214013 * seed + 2531011);
	return (seed >> 16) & 0x7FFF;
}

inline void ChangeCell(int x, int y, int type)
{
	int pos = x + y * bufferWidth;

	if (ChangedCells[pos] > 0)
	{
		//AllCellCounts[ChangedCells[pos]] -= 1;
	}

	ChangedCells[pos] = type;
	//CellDirection[pos] = !CellDirection[pos];

	if (type > 0)
	{
		//AllCellCounts[type] += 1;
	}
}

inline void ChangeCellDirect(int x, int y, int type)
{
	int pos = x + y * bufferWidth;

	if (Cells[pos] > 0)
	{
		AllCellCounts[Cells[pos]] -= 1;
	}

	Cells[pos] = type;

	if (type > 0)
	{
		AllCellCounts[type] += 1;
	}
}

inline void ChangeSurroundingCells(int x, int y, int type)
{
	if (x + 1 < bufferWidth)
	{
		ChangeCell(x + 1, y, type);
	}
	if (x - 1 >= 0)
	{
		ChangeCell(x - 1, y, type);
	}
	if (y + 1 < bufferHeight)
	{
		ChangeCell(x, y + 1, type);
	}
	if (y - 1 >= 0)
	{
		ChangeCell(x, y - 1, type);
	}
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	//Create
	WNDCLASS windowClass = {};
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpszClassName = "GWclass";
	windowClass.lpfnWndProc = windowCallback;

	//Register
	RegisterClass(&windowClass);

	//Window
	HWND window = CreateWindow(windowClass.lpszClassName, "Sand simulation", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1080, 0, 0, hInstance, 0);
	HDC hdc = GetDC(window);

	int test = 0;

	AllCellCounts[0] = bufferHeight * bufferWidth;

	Cells.resize(bufferHeight * bufferWidth);
	DeactiveCells.resize(bufferHeight * bufferWidth);

	for (int y = 0; y < bufferHeight; y++)
	{
		for (int x = 0; x < bufferWidth; x++)
		{
			Cells[x + y * bufferWidth] = 0;
			DeactiveCells[x + y * bufferWidth] = 0;
		}
	}

	for (int y = 0; y < bufferHeight; y++)
	{
		for (int x = 0; x < bufferWidth; x++)
		{
			if (y > 100)
			{
				if (rand() % 10 == 1)
				{
					Cells[x + y * bufferWidth] = rand() % 100 + 10;
				}

				//200, 300, 300, 500

				if (x > 200 && x < 250 && y > 300 && y < 1000)
				{
					Cells[x + y * bufferWidth] = 500 + ((float)(y + x) / 10.0f);
				}

				if (x > 1670 && x < 1700 && y > 500 && y < 900)
				{
					Cells[x + y * bufferWidth] = 500 + ((float)(y + x) / 10.0f);
				}

				//500, 1500, 600, 700

				if (x > 1010 && x < 1300 && y > 600 && y < 700)
				{
					Cells[x + y * bufferWidth] = -1;
				}

				if (x > 900 && x < 1000 && y > 600 && y < 700)
				{
					Cells[x + y * bufferWidth] = -1;
				}

				if (x > 1000 && x < 1010 && y > 540 && y < 550)
				{
					Cells[x + y * bufferWidth] = -1;
				}

				//700, 800, 900, 1000

				if (x > 700 && x < 1500 && y > 800 && y < 1000)
				{
					Cells[x + y * bufferWidth] = 500 + ((float)(y + x) / 10.0f);
				}
			}
			else
			{
				Cells[x + y * bufferWidth] = 100;
			}
		}
	}

	//Cells[300 + 800 * bufferWidth] = 1;

	//Cell pointers

	int* CurrentCell = &Cells[0];
	int* CurrentChangedCell = &ChangedCells[0];

	//done

	while (running == true)
	{
		// Input
		MSG message;
		while (PeekMessage(&message, window, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		int repeat = 0;

		Frames += 1;

		//srand(time(NULL));

		TotalTime = TotalTime.zero();
		TotalTimeCount = 0;

		int typeFrame = 50000 - Frames;

		if (typeFrame < 2)
		{
			typeFrame = 1;
		}

		auto startTime = std::chrono::high_resolution_clock::now();

		CelltypesCount = 0;
		largestCellSize = 0;
		largestCellType = 0;

		if (GetAsyncKeyState(VK_CONTROL) < 0)
		{
			//500000

			if (GetAsyncKeyState(VK_LBUTTON) < 0)
			{
				POINT p;

				GetCursorPos(&p);

				int x = p.x;
				int y = p.y;

				for (int X = 0; X < 210; X++)
				{
					for (int Y = 0; Y < 210; Y++)
					{
						if (x + X + (bufferHeight - y + Y) * bufferWidth < Cells.size())
						{
							Cells[x + X + (bufferHeight - y + Y) * bufferWidth] = ((x + X) + (y + Y)) + 100;
						}
					}
				}
			}

			if (GetAsyncKeyState(VK_RBUTTON) < 0)
			{
				POINT p;

				GetCursorPos(&p);

				int x = p.x;
				int y = p.y;

				for (int X = 0; X < 210; X++)
				{
					for (int Y = 0; Y < 210; Y++)
					{
						if (x + X + (bufferHeight - y + Y) * bufferWidth < Cells.size())
						{
							if (rand() % 1000 == 0 && Cells[x + X + (bufferHeight - y + Y) * bufferWidth] == 0)
							{
								Cells[x + X + (bufferHeight - y + Y) * bufferWidth] = rand() % 100 + 10;
							}
						}
					}
				}
			}

			if (GetAsyncKeyState(VK_MBUTTON) < 0)
			{
				POINT p;

				GetCursorPos(&p);

				int x = p.x;
				int y = p.y;

				for (int X = 0; X < 210; X++)
				{
					for (int Y = 0; Y < 210; Y++)
					{
						if (x + X + (bufferHeight - y + Y) * bufferWidth < Cells.size())
						{
							Cells[x + X + (bufferHeight - y + Y) * bufferWidth] = 0;
						}
					}
				}
			}
		}
		else
		{
			if (GetAsyncKeyState(VK_LBUTTON) < 0)
			{
				POINT p;

				GetCursorPos(&p);

				int x = p.x;
				int y = p.y;

				for (int X = 0; X < 70; X++)
				{
					for (int Y = 0; Y < 70; Y++)
					{
						if (x + X + (bufferHeight - y + Y) * bufferWidth < Cells.size())
						{
							Cells[x + X + (bufferHeight - y + Y) * bufferWidth] = ((x + X) + (y + Y)) + 100;
						}
					}
				}
			}

			if (GetAsyncKeyState(VK_RBUTTON) < 0)
			{
				POINT p;

				GetCursorPos(&p);

				int x = p.x;
				int y = p.y;

				for (int X = 0; X < 50; X++)
				{
					for (int Y = 0; Y < 50; Y++)
					{
						if (x + X + (bufferHeight - y + Y) * bufferWidth < Cells.size())
						{
							Cells[x + X + (bufferHeight - y + Y) * bufferWidth] = -1;
						}
					}
				}
			}

			if (GetAsyncKeyState(VK_MBUTTON) < 0)
			{
				POINT p;

				GetCursorPos(&p);

				int x = p.x;
				int y = p.y;

				for (int X = 0; X < 70; X++)
				{
					for (int Y = 0; Y < 70; Y++)
					{
						if (x + X + (bufferHeight - y + Y) * bufferWidth < Cells.size())
						{
							Cells[x + X + (bufferHeight - y + Y) * bufferWidth] = 0;
						}
					}
				}
			}
		}

		if (Update < 0.0f)
		{
			auto UpdateStartTime = std::chrono::high_resolution_clock::now();

			std::chrono::duration<float> UpdateTotalTime = UpdateTotalTime.zero();
			int UpdateTotalTimeCount = 0;

			CurrentChangedCell = CurrentCell;

			if (TotalCount < 0)
			{
				TotalCount -= 1;

				UpdateFirst = !UpdateFirst;

				//Simple sand simulation
				if (1 == 1)
				{
					int UpdateMulti = 1;
					int UpdateConst = 0;

					if (UpdateFirst == true)
					{
						UpdateMulti = -1;
						UpdateConst = bufferWidth;
					}
					else
					{
						UpdateMulti = 1;
					}

					for (int y = 100; y < bufferHeight; y++)
					{
						int Y1 = y * bufferWidth;
						int Y2 = (y - 1) * bufferWidth;

						for (int X = 0; X < bufferWidth; X++)
						{
							int x = UpdateConst + X * UpdateMulti;

							int CellType = *(CurrentCell + x + Y1);

							if (CellType != 0 && CellType != -1)
							{
								largestCellSize += 1;

								if (*(CurrentChangedCell + x + Y2) == 0)
								{
									*(CurrentChangedCell + x + Y1) = 0;
									*(CurrentChangedCell + x + Y2) = CellType;
								}
								else if (*(CurrentChangedCell + x - 1 + Y2) == 0)
								{
									*(CurrentChangedCell + x + Y1) = 0;
									*(CurrentChangedCell + x - 1 + Y2) = CellType;
								}
								else if (*(CurrentChangedCell + x + 1 + Y2) == 0)
								{
									*(CurrentChangedCell + x + Y1) = 0;
									*(CurrentChangedCell + x + 1 + Y2) = CellType;
								}
							}
						}
					}
				}
			}

			//Update = 1.0f;
			Update = 0.0f;

			//Cells = ChangedCells;

			CurrentCell = CurrentChangedCell;

			auto UpdateEndTime = std::chrono::high_resolution_clock::now();
			UpdateTotalTime += UpdateEndTime - UpdateStartTime;
			UpdateTotalTimeCount += 1;

			//std::chrono::milliseconds TotalTime2 = std::chrono::duration_cast<std::chrono::milliseconds>(TotalTime);
			std::chrono::microseconds UpdateTotalTime2 = std::chrono::duration_cast<std::chrono::microseconds>(UpdateTotalTime);

			std::ostringstream str;

			str << UpdateTotalTime2.count() / 1000.0f << " ms";

			OutputDebugString("Update time: ");
			OutputDebugString(str.str().c_str());
			OutputDebugString("|");
			OutputDebugString("\n");
		}

		// Render
		StretchDIBits(hdc, 0, 0, bufferWidth, bufferHeight, 0, 0, bufferWidth, bufferHeight, &Cells[0], &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);

		if (test > bufferWidth)
		{
			test = 0;
		}

		test++;

		// Time

		auto endTime = std::chrono::high_resolution_clock::now();
		TotalTime += endTime - startTime;
		TotalTimeCount += 1;

		//std::chrono::milliseconds TotalTime2 = std::chrono::duration_cast<std::chrono::milliseconds>(TotalTime);
		std::chrono::microseconds TotalTime2 = std::chrono::duration_cast<std::chrono::microseconds>(TotalTime);

		DeltaTime = 1.0f / (TotalTime2.count() / 1000000.0f);

		std::ostringstream timeString;

		timeString << TotalTime2.count() / (float)TotalTimeCount / 1000.0f << " ms" << "| Current fps: " << 1000000.0f / TotalTime2.count() << "| Cell types: " << CelltypesCount << "| Largest cell: " << largestCellSize << " | type: " << largestCellType;

		OutputDebugString("Total time: ");
		OutputDebugString(timeString.str().c_str());
		OutputDebugString("|");
		OutputDebugString("\n");

		Update -= 1.0f / (DeltaTime / 20.0f);

		RenderUpdate += 1.0f / (DeltaTime / 100);
		EventUpdate += 1.0f / (DeltaTime / 100);
	}
}
