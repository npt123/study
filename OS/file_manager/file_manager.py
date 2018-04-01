import os
from user import *


def main():
    while True:
        status = False
        print('1) Login')
        print('2) Create New User')
        choice = input('Input your choice:')
        if choice == '2':
            create_user()
        username = input("User:")
        user_list = open('shadow', 'r')
        for line in user_list.readlines():
            if line.split(':')[1] == username:
                status = True
                break
        user_list.close()
        if not status:
            print("User doesn't exist!")
            continue
        else:
            user = User(username)
            path = '/'
            if not user.login():
                break
            else:
                print('1) touch: 创建文件')
                print('2) mkdir: 创建目录')
                print('3) rm [-r]: 删除文件（目录）')
                print('4) open: 打开文件')
                print('5) close: 关闭文件')
                print('6) read: 读文件')
                print('7) write: 写文件')
                print('8) exit: 退出')
                while True:
                    command = input(path + username + ' ')
                    operate = command.split()[0]
                    if operate == 'exit':
                        user.logout()
                        break
                    elif operate == 'cd':
                        if os.path.exists('./root' + command.split()[1]):
                            path = command.split()[1]
                        else:
                            print('Path Error!')
                    elif operate == 'touch':
                        pass
                    elif operate == 'mkdir':
                        pass
                    elif operate == 'rm':
                        pass
                    elif operate == 'open':
                        pass
                    elif operate == 'close':
                        pass
                    elif operate == 'read':
                        pass
                    elif operate == 'write':
                        pass


if __name__ == '__main__':
    main()
