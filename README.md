**wl_sherlock** is a simple tool to parse, filter and understand wayland protocol logs.

<picture>
  <source srcset="screenshot.png 2x" />
  <img alt="Screenshot of wl_sherlock" src="screenshot.png" />
</picture>

## How to use

### Filter messages

Using the text field on top you can filter messages based on the interface or the id
of the object that's being talked about or based on the request being send or the event
being received. So for example

```
xdg_toplevel
```

would select all messages for the `xdg_toplevel` interface. Selecting messages based on the
object id is done by prefixing a number with `@` or `#`.

```
@31
```

Filtering requests or events is done by using a `.` in front of a name.

```
.configure
```

Of course these can all be combined. The following filter

```
wl_surface@20.commit
```

will select all `.commit()` messages for the object with id 20 of the
`wl_surface` interface.

You can chain messages. The filter will then select all messages from the
objects that the previous message created. So for example filtering all
`done()` events of the `sync()` request you can do

```
wl_display.sync.done
```

### Graph view

The graph view at the bottom of the window will always show the delta time
between all filtered messages. If the filter is empty it shows the time
difference between all messages. So if combined that with a filter on the
`.commit()` request it can be used to measure the framerate at which the
application is rendering.

## How to build

### Linux and macOS

```shell
$ git clone https://github.com/jrng/cui.git
$ cd cui
$ cc -o c_make c_make.c
$ ./c_make setup build build_type=reldebug
$ ./c_make build build
$ mkdir -p $HOME/opt/cui/include
$ mkdir -p $HOME/opt/cui/lib64
$ cp include/cui.h $HOME/opt/cui/include/
$ install -m 755 build/libcui.a $HOME/opt/cui/lib64/libcui.a
$ cd ..
$ git clone https://github.com/jrng/wl_sherlock.git
$ cd wl_sherlock
$ cc -o c_make c_make.c
$ ./c_make setup build build_type=reldebug
$ ./c_make build build
```

### Windows

```shell
$ git clone https://github.com/jrng/cui.git
$ cd cui
$ cl -Fec_make.exe c_make.c
$ c_make.exe setup build build_type=reldebug
$ c_make.exe build build
$ mkdir c:\\cui
$ copy include\\cui.h c:\\cui\\cui.h
$ copy build\\cui.lib c:\\cui\\cui.lib
$ cd ..
$ git clone https://github.com/jrng/wl_sherlock.git
$ cd wl_sherlock
$ cl -Fec_make.exe c_make.c
$ c_make.exe setup build build_type=reldebug
$ c_make.exe build build
```
