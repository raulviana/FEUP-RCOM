# FEUP-RCOM
Repository for the "Computer Networks" course [(**RCOM**)](https://sigarra.up.pt/feup/pt/ucurr_geral.ficha_uc_view?pv_ocorrencia_id=459483) pratical projects.

## Project 1 - [Data Connection Protocol](https://github.com/raulviana/FEUP-RCOM/blob/master/trabalho1/lig_dados_desempenho.pdf)
The first project goal was to implement a data connection protocol to transfer a file over an RS-232 serial port connection.

The first project goal was to implement a data connection protocol to transfer a file over an RS-232 serial port connection.

It's possible to emulate the physical serial connection between two computers using the **socat** command:

```sh
sudo socat -d -d PTY,link=/dev/ttyS10,mode=777 PTY,link=/dev/ttyS11,mode=777
```

Then, in different terminals, use the emitter connected to the ttyS10 port and the receiver connected to port ttyS11.

## Project 2 - [Network Configuration](https://github.com/raulviana/FEUP-RCOM/blob/master/trabalho2/lab2.pdf)

Project 2 goals were to implement an FTP download app and to configure a computer network.

The FTP download app has to connect to an FTP server and download a file. This app is also used to validate the proper configuration of the network.
