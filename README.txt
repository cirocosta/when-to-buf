
  USAGE:

        make

        ./server <rx_bufsize>
        ./client <address> <tx_bufsize>


  ARGUMENTS

        <(tx|rx)_bufsize>       size of the glibc internal buffers to
                                to be used when performing reads or writes
                                on file streams (via `stdio` operations).

                                > 0 :   buffer up to <tx_bufsize>
                                < 1 :   unbuffered writes

        <address>               IP address to connect to (without port)

  MORE

        read more at https://ops.tips/blog/when-to-buffer-writes/



