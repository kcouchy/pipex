/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bonus_pipex.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/11 11:06:41 by kcouchma          #+#    #+#             */
/*   Updated: 2024/01/26 11:20:11 by kcouchma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	ft_bonus_pipex(t_pipex *pipex)
{
	int	i;

	i = 0;
	while (i < pipex->commands)
	{
		if (i < (pipex->commands - 1))
		{
			if (pipe(pipex->pipe_fd) == -1)
			{
				perror("pipe failed");
				ft_freetable(pipex->paths);
				exit(EXIT_FAILURE);
			}
		}
		ft_bonus_forkchild(pipex, i);
		if (i == 0)
			pipex->pid_last = pipex->pid;
		if (i < (pipex->commands - 1))
			close(pipex->pipe_fd[0]);
		i++;
	}
	if (pipex->pid != 0)
		ft_wait_parent(pipex);
}

void	ft_heredoc(t_pipex *pipex)
{
	char	*buffer;

	buffer = NULL;
	pipex->heredoc = 1;
	pipex->commands -= 1;
	pipex->infile_fd = open("/tmp/temp", O_RDWR | O_TRUNC | O_CREAT, 0644);
	if (pipex->infile_fd == -1)
		ft_input_fail(pipex);
	while (1)
	{
		write(STDERR_FILENO, "> ", 2);
		buffer = get_next_line(STDIN_FILENO);
		if (!buffer)
			ft_byedoc(pipex);
		buffer[ft_strlen(buffer) - 1] = '\0';
		if (ft_strncmp(buffer, pipex->args[2], ft_strlen(buffer) + 1) == 0)
			break ;
		buffer[ft_strlen(buffer)] = '\n';
		write(pipex->infile_fd, buffer, ft_strlen(buffer));
		free(buffer);
	}
	(free(buffer), close(pipex->infile_fd));
}

void	ft_pipex_init(t_pipex *pipex, int argc, char **argv, char **envp)
{
	pipex->commands = argc - 3;
	pipex->envp = envp;
	pipex->args = argv;
	pipex->infile = argv[1];
	pipex->infile_fd = -1;
	pipex->outfile_fd = -1;
	pipex->outfile = argv[argc - 1];
	pipex->child_args = NULL;
	pipex->temp_fd_out = -1;
	pipex->heredoc = 0;
	pipex->exit_code = 0;
	pipex->paths = ft_extract_envp(envp);
}

int	main(int argc, char **argv, char **envp)
{
	t_pipex	pipex;

	ft_pipex_init(&pipex, argc, argv, envp);
	if (argc >= 5 && ft_strncmp(argv[1], "here_doc", 8) == 0)
		ft_heredoc(&pipex);
	if (argc >= 5 || (argc >= 6 && pipex.heredoc == 1))
		ft_bonus_pipex(&pipex);
	else if (pipex.heredoc == 0)
	{
		ft_printf("pipex input error:\n");
		ft_printf("./bonus_pipex infile cmd1 ... cmdn outfile\n");
		ft_printf("./bonus_pipex here_doc LIMITER cmd1 ... cmdn outfile\n");
	}
	else if (pipex.heredoc == 1)
	{
		ft_printf("pipex input error: ");
		ft_printf("./bonus_pipex here_doc LIMITER cmd1 ... cmdn outfile\n");
	}
	if (pipex.heredoc == 1 && pipex.infile_fd != -1)
		(close(pipex.infile_fd), unlink("temp"));
	ft_freetable(pipex.paths);
	return (pipex.exit_code);
}
