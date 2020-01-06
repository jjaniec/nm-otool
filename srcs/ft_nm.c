/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_nm.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/29 15:45:19 by jjaniec           #+#    #+#             */
/*   Updated: 2020/01/06 19:44:12 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

static int			read_and_process_file(char *filename)
{
	t_ft_nm_file	file;
	char			*file_content;

	if ((file.totsiz = read_file_content(filename, &file_content)) == -1)
		return (1);
	dprintf(2, "Read file total size: %d\n", file.totsiz);
	file.content = file_content;
	file.seek_ptr = file.content;
	return (ft_nm_process_file(&file));
}

int					main(int ac, char **av)
{
	int				i;
	int				r;

	if (ac == 1)
		return (read_and_process_file("a.out"));
	i = 0;
	while (++i != ac)
	{
		if (ac != 2)
			ft_printf("\n%s:\n", av[i]);
		if ((r = read_and_process_file(av[i])))
			return (r);
		// if (ac != 2)
	}
	return (0);
}
