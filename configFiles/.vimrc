" 定义快捷键的前缀, 即<Leader>
let mapleader=";"

" set plugins manager
set nocompatible
filetype off

set rtp+=$HOME/.vim/bundle/vundle
call vundle#begin()

Plugin 'gmarik/vundle'

" golang plugin
Plugin 'jnwhiteh/vim-golang'

" comments, press 'gcc' to comment the line, 'gc' to comment visual part.
Plugin 'tpope/vim-commentary'

" html quickly generate, 'html:5' and press '<C-Y>,'
Plugin 'mattn/emmet-vim'

" using Markdown
Plugin 'godlygeek/tabular'
Plugin 'plasticboy/vim-markdown'

" *.c 和 *.h 间切换
Plugin 'derekwyatt/vim-fswitch'

" " ctags 自动生成
" Plugin 'vim-scripts/DfrankUtil'
" Plugin 'vim-scripts/vimprj'
" Plugin 'vim-scripts/indexer.tar.gz'

" 搜索
Plugin 'dyng/ctrlsf.vim'

" 多光标
Plugin 'terryma/vim-multiple-cursors'

" " 语法检查
" Plugin 'scrooloose/syntastic'

" 快速打开
Plugin 'ctrlpvim/ctrlp.vim'

" 状态栏
Plugin 'Lokaltog/vim-powerline'

call vundle#end()

filetype on
filetype plugin on

" 语法高亮
syntax on

set hlsearch
set number

" 自适应不同语言的智能缩进 
filetype indent on
" 将制表符扩展为空格
set expandtab
" 设置编辑时制表符占用空格数
set tabstop=4
" 设置格式化时制表符占用空格数
set shiftwidth=4
" 让 vim 把连续数量的空格视为一个制表符
set softtabstop=4
set autoindent
set cindent
set backspace=indent,eol,start

" 设置风格
color desert
set background=dark

" 设置编码
let &termencoding=&encoding
set fileencodings=utf-8,gbk

" 让配置变更立即生效
autocmd BufWritePost $MYVIMRC source $MYVIMRC

" vim 自身命令行模式补全
set wildmenu

" *.c 和 *.h 间切换
nnoremap <silent> <Leader>sw :FSHere<CR>

" 标签
" 正向遍历同名标签
nnoremap <Leader>tn :tnext<CR>
" 反向遍历同名标签
nnoremap <Leader>tp :tprevious<CR>

" 使用 ctrlsf.vim 插件在工程内全局查找光标所在关键字. 
nnoremap <Leader>sp :CtrlSF<CR>

" 启用: Man命令查看各类man信息
source $VIMRUNTIME/ftplugin/man.vim
" 定义: Man命令查看各类man信息的快捷键
nnoremap <Leader>man :Man 3 <cword><CR>

" " 自动生成 ctags. 配置文件在 ~/.indexer_files
" " 头文件，在~/.vim/systags
" set tags+=~/.vim/systags

" 设置状态栏主题风格
let g:Powerline_colorscheme='solarized256'

" windows 下不备份
if has("win32")
    set nobackup
endif

" " 每次自动调用 :SyntasticSetLocList, 将错误覆盖 **quickfix**
" let g:syntastic_always_populate_loc_list = 1
" " 自动拉起/关闭错误窗口, 不需要手动调用 :Errors
" let g:syntastic_auto_loc_list = 1
" " 打开文件的时候做检查
" let g:syntastic_check_on_open = 1
" " 每次保存的时候做检查
" let g:syntastic_check_on_wq = 1
" " 错误窗口高度
" let g:syntastic_loc_list_height=5

" set statusline+=%#warningmsg#
" set statusline+=%{SyntasticStatuslineFlag()}
" set statusline+=%*

" 打开文件
nnoremap <Leader>op :CtrlP<CR>

" go 环境, 禁止自动下载
let g:go_disable_autoinstall = 0

" 記住上次游標的位置
if has("autocmd")
au BufReadPost * if line("'\"") > 1 && line("'\"") <= line("$") | exe "normal! g'\"" | endif
endif
