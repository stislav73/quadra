
from google.appengine.ext import webapp
from google.appengine.ext.webapp.util import run_wsgi_app

import qserv

application = webapp.WSGIApplication([('/qserv/', qserv.QServHandler)],
																		 debug=True)

def main():
	run_wsgi_app(application)

if __name__ == '__main__':
	main()
