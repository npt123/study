#!/usr/bin/python
import ftplib

def returnDefault(ftp):
    try:
        dirList = ftp.nlst()
    except:
        dirList = []
        print "[-] Could not list directory contents."
        print "[-] Skipping To Next Target."
        return
    retList = []
    for fileName in dirList:
        fn = fileName.lower()
        if ('.php' in fn) | ('.htm' in fn) | ('asp' in fn):
            print '[+] Found default page: ' + fileName
            retList.append(fileName)
    return retList

host = "192.168.1.186"
userName = "pi"
passWord = "yellowsea"
ftp = ftplib.FTP(host)
ftp.login(userName, passWord)
returnDefault(ftp)
