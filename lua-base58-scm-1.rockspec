package = 'lua-base58'
version = 'scm-1'

source  = {
    url    = 'git://github.com/Mons/lua-base58.git';
    branch = 'master';
}

description = {
    summary  = "Memcached protocol module for tarantool";
    detailed = [[
    Memcached protocol module for tarantool
    ]];
    homepage = 'https://github.com/Mons/lua-base58.git';
    license  = 'BSD';
    maintainer = "Mons Anderson <mons@cpan.org>";
}

dependencies = {
    'lua >= 5.1';
}

build = {
    type = 'builtin',
    modules = {
        ['base58'] = 'base58.lua';
        ['libbase58'] = {
            sources = {
                "libbase58.c",
            };
        }
    }
}