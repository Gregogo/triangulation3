#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include <string>
#include <optional>
#include <vector>
#include <cerrno>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

struct Vertex
{
	int x;
	int y;
	Vertex(int x, int y) : x(x), y(y) {};
};

float getK(Vertex A, Vertex B)
{
	if (B.x - A.x == 0)
	{
		return INT_MAX;
	}
	return (B.y - A.y) / (B.x - A.x);
}

struct Edge
{
	Vertex firstPoint;
	Vertex secondPoint;
	float prevSide;
	Edge(Vertex firstPoint, Vertex secondPoint) : firstPoint(firstPoint), secondPoint(secondPoint) {
		prevSide = 0;
	};
};

class Triangulation
{
public:
	Triangulation(std::vector <Vertex> vertexes)
	{
		this->vertexes = vertexes;
		buildGraph();
	}
	std::vector <Edge> allEdges;
private:
	void addEdge(Edge edge, float side);
	void buildGraph();
	std::vector <Vertex> vertexes;
	std::vector <Edge> activeEdges;
};

int FindEdge(Edge edge, std::vector<Edge> edges) {
	for (int i = 0; i < edges.size(); i++)
	{
		if (
			(edges[i].firstPoint.x == edge.firstPoint.x
				&& edges[i].firstPoint.y == edge.firstPoint.y
				&& edges[i].secondPoint.x == edge.secondPoint.x
				&& edges[i].secondPoint.y == edge.secondPoint.y)
			|| (edges[i].firstPoint.x == edge.secondPoint.x
				&& edges[i].firstPoint.y == edge.secondPoint.y
				&& edges[i].secondPoint.x == edge.firstPoint.x
				&& edges[i].secondPoint.y == edge.firstPoint.y)
			)
		{
			return i;
		}
	}
	return -1;
}

int FindVertex(Vertex vertex, std::vector<Vertex> vertexes) {
	for (int i = 0; i < vertexes.size(); i++)
	{
		if (vertex.x == vertexes[i].x && vertex.y == vertexes[i].y)
		{
			return i;
		}
	}
	return -1;
}

void Triangulation::addEdge(Edge edge, float side)
{

	auto foundAllEdge = FindEdge(edge, allEdges);
	if (foundAllEdge == -1)
	{
		allEdges.push_back(edge);
	}

	auto foundActiveEdge = FindEdge(edge, activeEdges);
	if (foundActiveEdge == -1 && foundAllEdge == -1)
	{
		activeEdges.push_back(edge);
		activeEdges[activeEdges.size() - 1].prevSide = side;
	}
	else
	{
		if (foundActiveEdge == -1)
		{
			//activeEdges.erase(activeEdges.cbegin() + foundActiveEdge);
			//activeEdges.push_back(edge);
		}
	}
}

void Triangulation::buildGraph()
{
	if (vertexes.size() < 2) {
		return;
	}
	//if (vertexes.size() == 3) {
	//	addEdge(Edge(vertexes[0], vertexes[1]));
	//	addEdge(Edge(vertexes[1], vertexes[2]));
	//	addEdge(Edge(vertexes[2], vertexes[0]));
	//	return;
	//}
	int currentX = vertexes[0].x;
	int currentY = vertexes[0].y;
	int CurrentAIndex = 0;
	for (int i = 1; i < vertexes.size(); i++)
	{
		if (vertexes[i].x < currentX)
		{
			CurrentAIndex = i;
		}
		if (vertexes[i].x == currentX)
		{
			if (vertexes[i].y > currentY)
			{
				CurrentAIndex = i;
			}
		}
	}
	Vertex A = vertexes[CurrentAIndex];
	vertexes.erase(vertexes.cbegin() + CurrentAIndex);

	float currentK = getK(A, vertexes[0]);
	int CurrentBIndex = 0;
	for (int i = 1; i < vertexes.size(); i++)
	{
		if (getK(A, vertexes[i]) < currentK)
		{
			currentK = getK(A, vertexes[i]);
			CurrentBIndex = i;
		}
	}
	Vertex B = vertexes[CurrentBIndex];
	vertexes.erase(vertexes.cbegin() + CurrentBIndex);

	addEdge(Edge(A, B), 0);

	while (activeEdges.size() > 0)
	{
		Edge currentEdge = activeEdges[0];
		std::cout << '(' << currentEdge.firstPoint.x << ' ' << currentEdge.firstPoint.y << ") (" << currentEdge.secondPoint.x << ' ' << currentEdge.secondPoint.y << ')' << '\n';
		auto begin = activeEdges.cbegin();
		activeEdges.erase(begin);

		auto begin2 = vertexes.cbegin();

		if (vertexes.size() > 0) {
			Vertex VectorMain = { currentEdge.secondPoint.x - currentEdge.firstPoint.x, currentEdge.secondPoint.y - currentEdge.firstPoint.y };
			float currentAngle = 0;
			float currentSide = 0;
			std::vector<int> currentIndexes;
			for (int i = 0; i < vertexes.size(); i++)
			{
				Vertex Vector1 = { currentEdge.firstPoint.x - vertexes[i].x, currentEdge.firstPoint.y - vertexes[i].y };
				Vertex Vector2 = { currentEdge.secondPoint.x - vertexes[i].x, currentEdge.secondPoint.y - vertexes[i].y };
				
				//if ((VectorMain.x * Vector2.y - VectorMain.y * Vector2.x > 0))
				if (currentEdge.prevSide <= 0 && (VectorMain.x * Vector2.y - VectorMain.y * Vector2.x > 0) >= 0
					|| currentEdge.prevSide >= 0 && (VectorMain.x * Vector2.y - VectorMain.y * Vector2.x > 0) <= 0)
				{
					float angleCos = (Vector1.x * Vector2.x + Vector1.y * Vector2.y)
						/ (std::sqrt(Vector1.x * Vector1.x + Vector1.y * Vector1.y) * std::sqrt(Vector2.x * Vector2.x + Vector2.y * Vector2.y));
					float angle = std::acos(angleCos);
					if (angle > currentAngle) {
						currentSide = (VectorMain.x * Vector2.y - VectorMain.y * Vector2.x > 0);
						currentAngle = angle;
						currentIndexes.clear();
						currentIndexes.push_back(i);
					}
					else if (angle == currentAngle) {
						std::cout << "the same" << '\n';
						currentIndexes.push_back(i);
					}
				}
			}
			if (currentIndexes.size() > 0)
			{
				for (int i = 0; i < currentIndexes.size(); i++)
				{
					if (i == 0)
					{
						addEdge(Edge(currentEdge.secondPoint, vertexes[currentIndexes[i]]), currentSide);
						addEdge(Edge(vertexes[currentIndexes[i]], currentEdge.firstPoint), currentSide);

						auto begin2 = vertexes.cbegin();

						auto begin = activeEdges.cbegin();
						if (FindEdge(Edge(currentEdge.firstPoint, currentEdge.secondPoint), activeEdges) != -1)
						{
							activeEdges.erase(begin + FindEdge(Edge(currentEdge.firstPoint, currentEdge.secondPoint), activeEdges));
						}
					}
					else
					{
						addEdge(Edge(vertexes[currentIndexes[i - 1]], vertexes[currentIndexes[i]]), currentSide);
						addEdge(Edge(vertexes[currentIndexes[i]], currentEdge.firstPoint), currentSide);


						auto foundIndex = FindEdge(Edge(vertexes[currentIndexes[i - 1]], currentEdge.firstPoint), activeEdges);
						if (foundIndex != -1)
						{
							auto begin = activeEdges.cbegin();
							activeEdges.erase(std::next(activeEdges.begin(), foundIndex));
						}
					}
				}
			}
		}
	}
}

int main() {
	std::vector<Vertex> vertexes = {

Vertex(122.72325188985982, 66.46690300445584),
Vertex(627.3726584892631, 296.0581199620134),
Vertex(258.6697515250187, 835.6496252076075),
Vertex(38.16845399690511, 533.8222943312476),
Vertex(543.655738173579, 795.4939607642095),
Vertex(887.7067955324881, 419.2239460148912),
Vertex(513.4630184942564, 410.9847966930955),
Vertex(70.8675695256713, 355.0818595333347),
Vertex(627.7351046603628, 135.64554886598182),
Vertex(742.5792270954407, 492.6897597397669),
Vertex(100.29564377507464, 839.3178359576924),
Vertex(758.0773696306426, 788.4385673250033),
Vertex(813.4719800976594, 360.69768623189435),
Vertex(500.70928120655645, 584.7964401499642),
Vertex(499.2358659695528, 659.4894793815658),
Vertex(783.000274454735, 303.34399007878903),
Vertex(487.3130444045228, 749.9605921195875),
Vertex(317.0257221439253, 872.3590905301324),
Vertex(123.5942034556013, 488.89653562909115),
Vertex(495.7054034840481, 362.3794672362301),
Vertex(756.3344918060822, 755.0403961104147),
Vertex(314.4130333016601, 349.0429519434173),
Vertex(425.66106537346997, 370.449535046344),
Vertex(237.44490935966857, 724.0661871027875),
Vertex(483.3488098800149, 404.6788557132509),
Vertex(652.2494780262863, 1.8298311123783018),
Vertex(489.9073403302091, 510.4031631615698),
Vertex(505.2234958386556, 328.94751856743926),
Vertex(535.6149210875018, 833.3315220222328),
Vertex(466.8617671238193, 589.3873439156879),
Vertex(765.9898454006667, 211.7350881712793),
Vertex(771.4460908206601, 734.2915500067503),
Vertex(801.5359133134922, 687.2016236563641),
Vertex(196.6281746645354, 559.3079394660806),
Vertex(128.7598354981214, 805.447378620005),
Vertex(78.62133323972111, 14.965214254751187),
Vertex(711.5939139037209, 747.4506578849706),
Vertex(476.073154220813, 772.5190568919088),
Vertex(504.916465020919, 850.6824103646409),
Vertex(133.49157606542528, 677.9031137203311),
	};

	Triangulation triangulation(vertexes);

	std::cout << "edges:" << '\n';
	for (auto edge : triangulation.allEdges)
	{
		std::cout << '(' << edge.firstPoint.x << ' ' << edge.firstPoint.y << ") (" << edge.secondPoint.x << ' ' << edge.secondPoint.y << ')' << '\n';
	}
	for (auto edge : triangulation.allEdges)
	{
		std::cout << "<ConnectionLine dot1={{x: " << edge.firstPoint.x <<", y: "<< edge.firstPoint.y << "}} dot2={{x: " << edge.secondPoint.x << ", y: " << edge.secondPoint.y << "}} color={'linear-gradient(90deg, rgba(255,167,50,1) 0%, rgba(255,167,50,0.303046218487395) 14%, rgba(255,167,50,0.3) 89%, rgba(255,167,50,1) 100%)'}></ConnectionLine>" << '\n';
	}
	return 0;
}
