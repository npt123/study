#!/usr/bin/python
import geoip2.database
gi = geoip2.database.Reader('/usr/local/share/GeoIP/GeoLite2-City.mmdb')
def printRecord(tgt):
    rec = gi.city(tgt)
    print rec
    # city = rec['city']
    # region = rec['region_name']
    # country = rec['country_name']
    # long = rec['longitude']
    # lat = rec['latitude']
    # print '[*] Target: ' + tgt + ' Geo-located'
    # print '[+] ' + str(city) + ', ' + str(region) + ', ' + str(country)
    # print '[+] Latitude: ' + str(lat) + ', Longitude: ' + str(long)
tgt = '106.14.173.38'
printRecord(tgt)
