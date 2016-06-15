struct Location
{
	int x;
	int y;

	Location(){}
	Location(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
	const bool operator == (const Location &o)
	{
		return (o.x == this->x && o.y == this->y);
	}
	const bool operator != (const Location &o)
	{
		return !(*this == o);
	}
	const Location operator + (const Location &o)
	{
		return Location(this->x + o.x, this->y + o.y);
	}

	const Location operator - (const Location &o)
	{
		return Location(this->x - o.x, this->y - o.y);
	}

	const Location operator = (const Location &o)
	{
		this->x = o.x;
		this->y = o.y;
		return *this;
	}

	const Location operator += (const Location &o)
	{
		this->x += o.x;
		this->y += o.y;
		return *this;
	}

	const Location operator -= (const Location &o)
	{
		this->x -= o.x;
		this->y -= o.y;
		return *this;
	}
};

class LocationPointerSimulator
{
public:
	~LocationPointerSimulator()
	{
		delete this->step->step->location;
		delete this->step->step;
		delete this->step;
	}

	Location* getLocation()
	{
		return step->step->location;
	}
	static LocationPointerSimulator* NewLocationPointer(Location* loc)
	{
		LocationPointerSimulator* locationPointer1 = new LocationPointerSimulator();
		LocationPointerSimulator2* locationPointer2 = new LocationPointerSimulator2();
		LocationPointerSimulator3* locationPointer3 = new LocationPointerSimulator3();
		locationPointer3->location = loc;

		locationPointer1->step = locationPointer2;
		locationPointer2->step = locationPointer3;
		return locationPointer1;
	}

private:
	struct LocationPointerSimulator3
	{
		char placeholders[32];
		Location* location;
	};

	struct LocationPointerSimulator2
	{
		char placeholders[64];
		LocationPointerSimulator3* step;
	};

	char placeholders[128];
	LocationPointerSimulator2* step;
};
