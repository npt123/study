import hashlib


class User:
    def __init__(self, name):
        self.name = name
        user_list = open('shadow', 'r')
        for (num, line) in enumerate(user_list):
            user = line.split(':')
            if user[1] == name:
                self.line_no = num
                self.user_id = user[0]
                self.password = user[2]
                self.group_id = user[3]
        user_list.close()
        self.status = False

    def login(self):
        for i in range(3):
            password = input("Password:")
            if hashlib.md5(password.encode()).hexdigest() == self.password:
                self.status = True
                return True
            else:
                print("The password is error!")
        return False

    def logout(self):
        self.status = False

    def change_username(self):
        name = input("Input new user name:")
        try:
            user_list = open('shadow', 'r+')
            user_list.readline(self.line_no).replace(self.name, name)
            user_list.close()
        except Exception:
            print('Error!')
            return False
        self.name = name
        return True

    def change_password(self):
        password = input('Origin password:')
        if hashlib.md5(password) == self.password:
            new_password = input('New password:')
            confirm_password = input('Confirm password:')
            if new_password == confirm_password:
                try:
                    user_list = open('user', 'r+')
                    user_list.readline(self.line_no).replace(self.password, hashlib.md5(new_password.encode()).hexdigest())
                    user_list.close()
                except Exception:
                    print('Error!')
                    return False
                self.password = new_password
                print('password changed success!')
        return True


def create_user():
    name = input("User name:")
    user_list = open('shadow', 'r')
    for line in user_list.readlines():
        if line.split(':')[1] == name:
            print("User has been created!")
            return False
    user_list.seek(0)
    user_id = int(user_list.readline(-1).split(':')[0]) + 1
    password = ''
    for i in range(3):
        password = input("Password:")
        if input("Confirm password:") != password:
            print("Password do not match!")
            if i == 2:
                return False
        else:
            break
    group = input("Group:")
    group_list = open('group', 'r')
    group_id = int(group_list.readline(-1).split(':')[0]) + 1
    for line in group_list.readlines():
        if line.split(':')[1] == group:
            group_id = line[0]
            break
    group_list = open('group', 'a')
    group_list.write(str(group_id) + ':' + group + '\n')
    group_list.close()
    user_list = open('shadow', 'a')
    user_list.write(str(user_id) + ':' + name + ':' +
                    hashlib.md5(password.encode()).hexdigest() + ':' + str(group_id) + '\n')
    user_list.close()
    print("Creating user success!")