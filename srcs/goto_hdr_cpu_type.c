/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   goto_hdr_cpu_type.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/21 15:13:52 by joffreyjani       #+#    #+#             */
/*   Updated: 2020/01/10 20:32:19 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

t_ft_nm_hdrinfo			*goto_hdr_cpu_type(t_ft_nm_hdrinfo *hdr_list, \
							cpu_type_t target_type)
{
	t_ft_nm_hdrinfo		*h;

	h = hdr_list;
	while (h && \
		(h->cpu_type != target_type || \
		!((h->cpu_subtype & 0xf) == CPU_SUBTYPE_X86_64_ALL || \
		(h->cpu_subtype & 0xf) == CPU_SUBTYPE_X86_ALL)))
		h = h->next;
	return (h);
}
