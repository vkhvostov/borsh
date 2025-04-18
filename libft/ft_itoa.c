#include "libft.h"

static char	*build_str(int n, int sign, int length)
{
	char	*str;

	str = (char *)malloc(length + 1);
	if (str == NULL)
		return (NULL);
	str[length] = '\0';
	while (length--)
	{
		str[length] = (n % 10) + '0';
		n /= 10;
	}
	if (sign)
		str[0] = '-';
	return (str);
}

char	*ft_itoa(int n)
{
	int		length;
	int		sign;
	int		n_copy;

	length = 1;
	sign = 0;
	if (n < 0)
	{
		if (n == -2147483648)
			return (ft_strdup("-2147483648"));
		sign = 1;
		length++;
		n = -n;
	}
	n_copy = n;
	while (n / 10)
	{
		length++;
		n /= 10;
	}
	return (build_str(n_copy, sign, length));
}
