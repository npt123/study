class File:
    content = ''

    def __init__(self, path, name, owner, group, dtype, o_access, g_access, t_access):
        self.path = path
        self.name = name
        self.owner = owner
        self.group = group
        self.dtype = dtype
        self.o_access = o_access
        self.g_access = g_access
        self.t_access = t_access

    def read(self, user):
        if user == self.owner:
            if self.o_access[0]:
                return self.content
        else:
            user_list = open('shadow', 'r')
            for line in user_list.readlines():
                if line.split(':')[0] == user:
                    group_id = line.split(':')[-1]
                    if group_id == self.group:
                        if self.g_access[0]:
                            return self.content
            if self.t_access[0]:
                return self.content
        return

    def write(self, user, content):
        if user == self.owner:
            if self.o_access[1]:
                self.content = content
                return True
        else:
            user_list = open('shadow', 'r')
            for line in user_list.readlines():
                if line.split(':')[0] == user:
                    group_id = line.split(':')[-1]
                    if group_id == self.group:
                        if self.g_access[1]:
                            self.content = content
                            return True
            if self.t_access[1]:
                self.content = content
                return True
        return False
