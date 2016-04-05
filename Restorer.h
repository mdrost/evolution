#ifndef RESTORER_H
#define RESTORER_H

template <class T>
class Restorer
{
public:
	explicit Restorer(T &obj)
		: ptr(&obj)
		, old(obj)
	{}

	Restorer(const Restorer& other) = delete;

	Restorer(Restorer&& other)
		: ptr(other.ptr)
		, old(other.old)
	{
		other.ptr = nullptr;
	}

	Restorer& operator=(const Restorer& other) = delete;

	Restorer& operator=(Restorer&& other)
	{
		ptr = other.ptr;
		old = other.old;
		other.ptr = nullptr;
	}

	~Restorer()
	{
		if (ptr) {
			restore();
		}
	}

	void restore()
	{
		*ptr = old;
	}

private:
	T* const ptr;
	const T old;
};

#endif // RESTORER_H
