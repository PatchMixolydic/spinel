Filesystem Layout
=================

.. note:: This article describes the layout of directories and files within the
    filesystem. Documentation on the structure of filesystem drivers is
    forthcoming.

.. note:: This part of Spinel has not been implemented. It is (especially)
    subject to change.

The layout of Spinel's filesystem layout is inspired by that of Unix-like
systems, especially by the
`Filesystem Hierarchy Standard <https://refspecs.linuxfoundation.org/fhs.shtml>`_.

----

:file:`/` is the root directory. Every file is a child of :file:`/`.

----

:file:`/bin` contains system binaries which are a core part of the Spinel
operating system. These include, among others, :command:`cat`, :command:`ls`,
:command:`man`, or :command:`dd`.

.. note:: Names are not finalized.

----

:file:`/boot` is where the bootloader's files are stored. :file:`/boot` will
typically be located on its own partition, though this is not required.

----

:file:`/cfg` contains configuration data.

.. note:: This is analogous to the :file:`/etc` directory on most Unix-like
    systems.

----

:file:`/dev` contains device files. Examples include :file:`/dev/null`,
:file:`/dev/floppy/0`, :file:`/dev/scsi/3p4`, and :file:`/dev/hwrng/rdrand`.

.. note:: Contrary to popular belief, :file:`/dev` does not store developers.

----

:file:`/home` is home to users' home directories. Home directories are personal
directories where a user can store their own data, such as pictures, documents,
executables, fanfiction, and code repositories.

Due to the large size of codebases, 4K images, and descriptions of how
the light plays out on a romantic partner's face, and to allow for easy
transitions between operating systems, it is highly recommended that the home
directory is placed on a separate partition.

.. note:: Storing nonfiction works within a home directory may cause
    undefined behaviour and may lead to repeated kernel panics, physical
    erasure of your disk, the apocalypse, and increased memory usage.

----

:file:`/mnt` should be used to store mount points for filesystems, whether
they be local or remote. It is recommended that filesystems should be mounted
to subfolders of :file:`/mnt`, and not to :file:`/mnt` itself.

.. note:: :file:`/media` in the FHS serves the same purpose as :file:`/mnt`.
    The former has thus not been included in Spinel.

----

:file:`/pkg` contains data managed by the package manager. The structure of
:file:`/pkg` is similar to that of :file:`/usr`.

.. note:: For more information, see :file:`/usr`.

----

:file:`/root` is the home directory for the root user. It is located outside
of :file:`/home` so that the system can still be accessed even if :file:`/home`
is inaccessible.

.. note:: Sometimes, it can feel like :file:`/home` is inaccessible, even
    though :command:`pwd` claims that you're in :file:`~`. This is normal.

----

:file:`/srv` holds files that will be served by this computer. Example usages
of :file:`/srv` include using it for web pages or for a ZFS pool.

.. note:: This is not intended as a guarantee that ZFS will be available on
    Spinel.

----

:file:`/tmp` holds temporary files. :file:`/tmp` will be cleared on every
restart. :file:`/tmp` will most likely be implemented as an in-RAM filesystem.

.. note:: Please please please please please do not store anything important in
    :file:`/tmp`.

----

:file:`/usr` contains data managed by the end user, such as applications which
have been installed system-wide.

.. note:: Unlike some Unix-like systems, Spinel does not place the home
    directories in :file:`/usr/home`.

:file:`/usr/bin` contains executables installed by the end user. Examples may
include text editors, games, and music players.

:file:`/usr/include` stores header files for development use, especially for
the C and C++ programming languages.

:file:`/usr/lib` stores library files for development use. It may also store
shared libraries.

.. note:: :file:`/lib` has been deliberately omitted.

:file:`/usr/res` stores architecture-independent data, such as themes, images,
fonts, and system configuration.

.. note:: This is analogous to :file:`/usr/share` on most Unix systems.

:file:`/usr/src` contains the source code for Spinel.

----

:file:`/var` contains files which are variable in nature and are constantly
changing. These include logs, print spools, and mailboxes.

:file:`/var/log` stores log files for applications and for the kernel.

:file:`/var/mail` stores mailboxes for users.
