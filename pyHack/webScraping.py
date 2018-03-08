import builtwith
import whois
import re
import urllib2
import itertools

def download(url, user_agent='wswp', num_retries=2):
    print 'Downloading:', url
    headers = {'User-agent': user_agent}
    request = urllib2.Request(url, headers=headers)
    try:
        html = urllib2.urlopen(request).read()
    except urllib2.URLError as e:
        print 'Download error:', e.reason
        html = None
        if num_retries > 0:
            if hasattr(e, 'code') and 500 <= e.code < 600:
                # retry 5XX HTTP errors
                return download(url, user_agent, num_retries-1)
    return html

def crawl_sitemap(url):
    # download the sitemap file
    sitemap = download(url)
    # extend the sitemap links
    links = re.findall('<loc>(.*?)</loc>', sitemap)
    # download each link
    for link in links:
        html = download(link)
        # scrape html here
        # ...

def link_crawler(seed_url, link_regex):
    """Crawl from the given seed URL following links matched by link_regex"""
    crawl_queue = [seed_url]
    while crawl_queue:
        url = crawl_queue.pop()
        html = download(url)
        # filter for links matching our regular expression
        for link in get_links(html):
            crawl_sitemap(link)

def get_links(html):
    """Return a list of links from html"""
    # a regular expression to extract all links from the webpage
    webpag_regex = re.compile('<a[^>]+href=["\'](.*?)["\']', re.IGNORECASE)
    # list f all links from the webpage
    return webpag_regex.findall(html)

def main():
    url = 'http://example.webscraping.com/sitemap.xml'
    link_crawler(url, '/(index|view)')

if __name__ == '__main__':
    main()