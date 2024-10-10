FROM debian:bullseye-slim

ENV DEBIAN_FRONTEND=noninteractive

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

# Configure zsh
COPY .zshrc /root/.zshrc


# Set the default command to zsh
CMD ["zsh"]