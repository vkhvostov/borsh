int	ft_atoi(const char *str)
{
	int	index;
	int	result;
	int	sign;

	index = 0;
	result = 0;
	sign = 1;
	while (str[index] == ' ' || (str[index] >= 9 && str[index] <= 13))
		index++;
	if (str[index] == '-' || str[index] == '+')
	{
		if (str[index] == '-')
			sign = -1;
		index++;
	}
	while (str[index] >= '0' && str[index] <= '9')
	{
		result = result * 10 + str[index] - '0';
		index++;
	}
	return (result * sign);
}

/* 
#include <stdio.h>
int main()
{
	printf("Input: \"123\" -> ft_atoi: %d, atoi: %d\n",
		ft_atoi("123"), atoi("123"));
	printf("Input: \"-123\" -> ft_atoi: %d, atoi: %d\n",
		ft_atoi("-123"), atoi("-123"));

	printf("Input: \"   456\" -> ft_atoi: %d, atoi: %d\n",
		ft_atoi("   456"), atoi("   456"));
	printf("Input: \"\t\n\v\f\r 789\" -> ft_atoi: %d, atoi: %d\n",
		ft_atoi("\t\n\v\f\r 789"), atoi("\t\n\v\f\r 789"));

	printf("Input: \"--123\" -> ft_atoi: %d, atoi: %d\n",
		ft_atoi("--123"), atoi("--123"));
	printf("Input: \"+123\" -> ft_atoi: %d, atoi: %d\n",
		ft_atoi("+123"), atoi("+123"));
	printf("Input: \"-+456\" -> ft_atoi: %d, atoi: %d\n",
		ft_atoi("-+456"), atoi("-+456"));
	printf("Input: \"++789\" -> ft_atoi: %d, atoi: %d\n",
		ft_atoi("++789"), atoi("++789"));

	printf("Input: \"123abc\" -> ft_atoi: %d, atoi: %d\n",
		ft_atoi("123abc"), atoi("123abc"));
	printf("Input: \"-456xyz\" -> ft_atoi: %d, atoi: %d\n",
		ft_atoi("-456xyz"), atoi("-456xyz"));

	printf("Input: \"\" -> ft_atoi: %d, atoi: %d\n",
		ft_atoi(""), atoi(""));
	printf("Input: \"   \" -> ft_atoi: %d, atoi: %d\n",
		ft_atoi("   "), atoi("   "));
	printf("Input: \"0\" -> ft_atoi: %d, atoi: %d\n",
		ft_atoi("0"), atoi("0"));
	printf("Input: \"-0\" -> ft_atoi: %d, atoi: %d\n",
		ft_atoi("-0"), atoi("-0"));

	printf("Input: \"2147483647\" -> ft_atoi: %d, atoi: %d\n",
		ft_atoi("2147483647"), atoi("2147483647"));
	printf("Input: \"-2147483648\" -> ft_atoi: %d, atoi: %d\n",
		ft_atoi("-2147483648"), atoi("-2147483648"));
	printf("Input: \"2147483648\" -> ft_atoi: %d, atoi: %d\n",
		ft_atoi("2147483648"), atoi("2147483648"));
	printf("Input: \"-2147483649\" -> ft_atoi: %d, atoi: %d\n",
		ft_atoi("-2147483649"), atoi("-2147483649"));

	printf("Input: \"9223372036854775807\" -> ft_atoi: %d, atoi: %d\n",
		ft_atoi("9223372036854775807"), atoi("9223372036854775807"));
	printf("Input: \"-9223372036854775808\" -> ft_atoi: %d, atoi: %d\n",
		ft_atoi("-9223372036854775808"), atoi("-9223372036854775808"));

	return 0;
}
 */