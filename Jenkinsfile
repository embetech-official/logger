pipeline {
    agent {
		label 'windows'
	}
    stages {
		stage('Checkout') {
			steps {
				checkout scm
			}
		}
		stage('Build and Test') {
			steps {
				powershell '''embeshell
				ctest --build-and-test . build --build-generator Ninja --test-command ctest
				'''
			}
		}
	}
	post {
        changed {
            emailext body: '''${SCRIPT, template="groovy-text.template"}''', recipientProviders: [culprits(),  buildUser()], subject: '[EMBETECH_JENKINS] ${DEFAULT_SUBJECT}'
        }    
    }
}
