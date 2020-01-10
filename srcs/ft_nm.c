/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_nm.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/29 15:45:19 by jjaniec           #+#    #+#             */
/*   Updated: 2020/01/10 20:34:05 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

static int			read_and_process_file(char *filename, bool print_filename)
{
	t_ft_nm_file	file;
	char			*file_content;

	if ((file.totsiz = read_file_content(filename, &file_content)) == -1)
		return (1);
	dprintf(2, "Read file total size: %d, %zu\n", \
		file.totsiz, sizeof(struct nlist_64));
	file.content = file_content;
	file.seek_ptr = file.content;
	file.filepath = filename;
	return (ft_nm_process_file(&file, print_filename));
}

int					main(int ac, char **av)
{
	unsigned int	i;
	int				r;

	if (ac == 1)
		return (read_and_process_file("a.out", false));
	i = 0;
	r = 0;
	while (++i != ac)
		r += read_and_process_file(av[i], ac != 1);
	return (r != 0);
}
