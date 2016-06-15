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