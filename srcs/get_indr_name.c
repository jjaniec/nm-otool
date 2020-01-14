/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_indr_name.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/14 19:04:57 by jjaniec           #+#    #+#             */
/*   Updated: 2020/01/14 19:05:16 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

char			*get_indr_name(t_ft_nm_file *file, uint64_t n_value, \
					char *strtab_offset, struct symtab_command *symtabcmd)
{
	if (n_value == 0)
		return ("");
	if (n_value > symtabcmd->strsize || \
		slseek(file, (int)(&strtab_offset[n_value] - file->content), \
		SLSEEK_SET) == -1)
		return ("bad string index");
	return (strtab_offset + n_value);
}
