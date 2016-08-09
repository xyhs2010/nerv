# antigen
source $HOME/.antigen.zsh

antigen use oh-my-zsh

antigen bundle git
antigen bundle zsh-users/zsh-syntax-highlighting

antigen apply

# colors
autoload colors
colors
 
for color in RED GREEN YELLOW BLUE MAGENTA CYAN WHITE; do
    eval _$color='%{$terminfo[bold]$fg[${(L)color}]%}'
    eval $color='%{$fg[${(L)color}]%}'
done

# vcs_info
autoload -Uz vcs_info
zstyle ':vcs_info:*' enable git svn
precmd() {
    vcs_info
}

# prompt
RPROMPT='$YELLOW${vcs_info_msg_0_}'
PROMPT=$(echo "$YELLOW%T %1/$_WHITE#$FINISH ")

# editor
export EDITOR=vim

# java
export JAVA_HOME=/Library/Java/JavaVirtualMachines/jdk1.8.0_92.jdk/Contents/Home/
export PATH=$JAVA_HOME/bin:$PATH
export CLASSPATH=.:$JAVA_HOME/lib/dt.jar:$JAVA_HOME/lib/tools.jar

# history
HISTSIZE=1000
SAVEHIST=1000
setopt hist_ignore_all_dups
