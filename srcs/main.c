/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/29 15:45:19 by jjaniec           #+#    #+#             */
/*   Updated: 2019/11/29 16:19:58 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

static char			*check_args(int ac, char **av)
{
	if (ac > 1)
		return (av[1]);
}

int		main(int ac, char **av)
{
	char	*file;

	if (!(file = check_args(ac, av)))
	{
		file = "a.out";
	}
	return (ft_nm(file));
}
