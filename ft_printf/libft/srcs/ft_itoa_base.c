/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa_base.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/07 16:09:54 by jjaniec           #+#    #+#             */
/*   Updated: 2019/09/18 16:39:10 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

static size_t	get_size_alloc(int n, unsigned int base)
{
	size_t			size_alloc;
	unsigned int	u;

	if (n == 0)
		return (1);
	size_alloc = (n < 0) ? (1) : (0);
	u = (n < 0) ? ((unsigned int)-n) : ((unsigned int)n);
	while (u > 0)
	{
		u = u / base;
		++size_alloc;
	}
	return (size_alloc);
}

static void		fill_str(char *str, int n, size_t size_alloc, unsigned int base)
{
	unsigned int	u;

	str[size_alloc] = '\0';
	if (n == 0)
	{
		*str = '0';
		return ;
	}
	u = (n < 0) ? ((unsigned int)-n) : ((unsigned int)n);
	while (u > 0)
	{
		--size_alloc;
		str[size_alloc] = u % base + '0';
		u = u / base;
	}
	if (n < 0)
		*str = '-';
}

char			*ft_itoa_base(int n, unsigned int base)
{
	char	*str;
	size_t	size_alloc;

	size_alloc = get_size_alloc(n, base);
	str = (char *)malloc(sizeof(char) * (size_alloc + 1));
	if (str == NULL)
		return (NULL);
	fill_str(str, n, size_alloc, base);
	return (str);
}
