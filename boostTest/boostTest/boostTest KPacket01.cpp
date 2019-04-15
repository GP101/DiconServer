#include <stdio.h>
#include <iostream>
#include <set>
#include <string>

struct KData
{
	char	_cData;
	float	_fData;
};

void main()
{
	int iSize = sizeof(KData);
	printf("%i\r\n", iSize);
}
