/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_nm.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/29 15:45:19 by jjaniec           #+#    #+#             */
/*   Updated: 2020/01/18 15:03:34 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

static int			read_and_process_file(char *filename, bool print_filename)
{
	t_ft_nm_file	file;
	char			*file_content;
	int				r;

	if ((file.totsiz = read_file_content(filename, &file_content)) == -1)
		return (1);
	file.content = file_content;
	file.seek_ptr = file.content;
	file.filepath = filename;
	r = ft_nm_process_file(&file, print_filename);
	free(file_content);
	return (r);
}

int					main(int ac, char **av)
{
	unsigned int	i;
	int				r;

	if (ac == 1)
		return (read_and_process_file("a.out", false));
	i = 0;
	r = 0;
	while ((int)++i != ac)
		r += read_and_process_file(av[i], ac != 2);
	return (r != 0);
}
