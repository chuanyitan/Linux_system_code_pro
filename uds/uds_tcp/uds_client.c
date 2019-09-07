#include <stdio.h>    
#include <sys/un.h>    
#include <sys/socket.h>    
#include <string.h>
#include <unistd.h>
    
int main(void)  
{    
    int i_fd = 0;    
    struct sockaddr_un addr;    
    char psz_path[32] = "./client_unixsocket_file";    
    char serverpath[32] = "./server_unixsocket_file";    
    int i_addr_len = sizeof( struct sockaddr_un );    
    char psz_wbuf[32] = "i am client.";
    char psz_rbuf[32] = {0}; 
    int i_readlen = 0;
  
    if ( ( i_fd = socket( AF_UNIX, SOCK_STREAM, 0 ) ) < 0 )  
    {    
        perror("socket");
        return -1;
    }
  
    memset( &addr, 0, sizeof( addr ) );    
    addr.sun_family = AF_UNIX;
    strncpy( addr.sun_path, psz_path, sizeof( addr.sun_path ) - 1 );    
    unlink( psz_path );
  
    if ( bind( i_fd, ( struct sockaddr * )&addr, i_addr_len ) < 0 )  
    {    
        perror("bind");
        return -1;
    }
  
 
    memset( &addr, 0, sizeof( addr ) );    
    addr.sun_family = AF_UNIX;
    strncpy( addr.sun_path, serverpath, sizeof( addr.sun_path ) - 1 );    
  
    if ( connect( i_fd, ( struct sockaddr * )&addr, i_addr_len ) < 0 )  
    {
        perror("connect");
        return -1;
    }
  
    if ( write( i_fd, psz_wbuf, strlen( psz_wbuf ) + 1 ) < 0 )  
    {
        perror( "write" );    
        return -1;
    }    
  
    if ( ( i_readlen = read( i_fd, psz_rbuf, sizeof( psz_rbuf ) - 1 ) ) < 0 )  
    {    
        perror("write");    
        return -1;    
    }
    psz_rbuf[i_readlen] = '\0';
    printf( "receive msg:%s\n", psz_rbuf );    
    unlink( psz_path );
    return -1;
}    

