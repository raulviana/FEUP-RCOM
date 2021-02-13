# FEUP-RCOM
Repository for the "Computer Networks" course [(**RCOM**)](https://sigarra.up.pt/feup/pt/ucurr_geral.ficha_uc_view?pv_ocorrencia_id=459483) pratical projects.

## Project 1 - [Data Connection Protocol](https://github.com/raulviana/FEUP-RCOM/blob/master/trabalho1/lig_dados_desempenho.pdf)
The first project goal was to implement a data connection protocol to transfer a file over an RS-232 serial port connection.

The first project goal was to implement a data connection protocol to transfer a file over an RS-232 serial port connection.

It's possible to emulate the physical serial connection between two computers using the **socat** command:

'''
sudo socat -d -d PTY,link=/dev/ttyS10,mode=777 PTY,link=/dev/ttyS11,mode=777
'''

Then, in different terminals, use the emitter connected to the ttyS10 port and the receiver connected to port ttyS11.

