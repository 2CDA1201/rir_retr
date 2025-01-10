from ftplib import FTP

RIR_DICT = {
    'ARIN': {
        'host': 'ftp.arin.net',
        'stats': '/pub/stats/arin',
        'file': 'delegated-arin-extended-latest'
    },
    'RIPE NCC': {
        'host': 'ftp.ripe.net',
        'stats': '/pub/stats/ripencc',
        'file': 'delegated-ripencc-latest'
    },
    'APNIC': {
        'host': 'ftp.apnic.net',
        'stats': '/pub/stats/apnic',
        'file': 'delegated-apnic-latest'
    },
    'LACNIC': {
        'host': 'ftp.lacnic.net',
        'stats': '/pub/stats/lacnic',
        'file': 'delegated-lacnic-latest'
    },
    'AFRINIC': {
        'host': 'ftp.afrinic.net',
        'stats': '/pub/stats/afrinic',
        'file': 'delegated-afrinic-latest'
    },
}

def retr_ftp(region):
    print(f'Connecting to {region['host']}')
    ftp = FTP(region['host'], user='anonymous', passwd='uuk65536@gmail.com')
    ftp.login()
    ftp.cwd(region['stats'])

    print(f'Retrieving {region['file']}')
    with open(f'data/{region['file']}.psv', 'wb') as fp:
        ftp.retrbinary(cmd=f'RETR {region['file']}', callback=fp.write)
    
    ftp.quit()

def main():
    for stats in RIR_DICT.values():
        retr_ftp(stats)
        
if __name__ == '__main__':
    main()