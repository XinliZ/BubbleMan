#pragma once

namespace KinectVisionLib
{
    namespace Core
    {
        union Point
        {
        private:
            uint64 data;
            struct {
                int x;
                int y;
            };

        public:
            Point(int x, int y)
            {
                this->x = x;
                this->y = y;
            }
            Point(const Point& point)
            {
                this->x = point.x;
                this->y = point.y;
            }

            int GetX() const { return x; }
            int GetY() const { return y; }

            Point LeftNeighbor() const
            {
                return Point(this->x - 1, this->y);
            }

            Point RightNeighbor() const
            {
                return Point(this->x + 1, this->y);
            }

            Point UpNeighbor() const
            {
                return Point(this->x, this->y - 1);
            }

            Point DownNeighbor() const
            {
                return Point(this->x, this->y + 1);
            }

            Point Offset(int dx, int dy) const
            {
                return Point(this->x + dx, this->y + dy);
            }

            Point Offset(Point delta) const
            {
                return Point(this->x + delta.x, this->y + delta.y);
            }
        };
    }
}