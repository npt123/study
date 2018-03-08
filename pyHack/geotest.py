import pygeoip
import geoip2.database
gi = pygeoip.GeoIP('/usr/local/share/GeoIP/GeoLite2-City.mmdb')
print gi.record_by_name('106.14.173.38')
