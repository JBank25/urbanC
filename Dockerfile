FROM debian:bullseye-slim

RUN apt-get update && apt-get install -y \
    gdb \
    zsh \
    fzf \
    bat \
    tmux \
    git \
    curl \
    wget \
    vim \
    ruby-full \
    make \
    exa \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Set up zsh as the default shell
RUN chsh -s /usr/bin/zsh

# Create .config/zsh directory
RUN mkdir -p /root/.config/zsh

# Clone zsh-syntax-highlighting repository
RUN git clone https://github.com/zsh-users/zsh-syntax-highlighting.git /root/.config/zsh/zsh-syntax-highlighting

# This needs some more looking at, actions run locally 
# RUN curl --proto '=https' --tlsv1.2 -sSf https://raw.githubusercontent.com/nektos/act/master/install.sh | bash

# Set the default command to zsh
CMD ["zsh"]
