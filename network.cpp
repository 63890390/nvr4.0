/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
bool NetInit() {
    struct sockaddr_in serv_addr;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cout << "ERROR: socket create failed: " << strerror(errno) << "\r\n";
        return false;
    }
    memset((char *) &serv_addr, 0, sizeof (serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(INPUTPORT);
    for (int count = 0; bind(sock, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0; count++) {
        cout << "ERROR: bind socket failed: " << strerror(errno) << " retry after " << RETRY_NETWORCK_REINIT_SECOND << "second\r\n";
        if (count > RETRY_NETWORCK_REINIT_COUNT) {
            cout << "The maximum number of network setup attempts has been reached.\r\n";
            return false;
        }
        std::this_thread::sleep_for(std::chrono::seconds(RETRY_NETWORCK_REINIT_SECOND));
    }
    if (sock <= 0) {
        cout << "ERROR: Critical network error! abort\r\n";
        return false;
    }

    int enable = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    cout << strerror(errno) << "\r\n";
    
    if (listen(sock, SOMAXCONN) == -1) {
        cout << "ERROR: socket listen failed: " << strerror(errno) << "\r\n";
        return false;
    }
    MainSocket = sock;
    return true;
}
  */