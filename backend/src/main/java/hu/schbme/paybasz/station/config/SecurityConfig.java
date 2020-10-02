package hu.schbme.paybasz.station.config;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Configuration;
import org.springframework.security.config.annotation.authentication.builders.AuthenticationManagerBuilder;
import org.springframework.security.config.annotation.web.builders.HttpSecurity;
import org.springframework.security.config.annotation.web.configuration.EnableWebSecurity;
import org.springframework.security.config.annotation.web.configuration.WebSecurityConfigurerAdapter;
import org.springframework.security.core.userdetails.UserDetailsService;

@Configuration
@EnableWebSecurity
public class SecurityConfig extends WebSecurityConfigurerAdapter {

    @Value("${paybasz.admin.username:admin}")
    private String adminUsername;

    @Value("${paybasz.admin.password:1234}")
    private String adminPassword;

    @Autowired
    private UserDetailsService userDetailsService;

    @Override
    protected void configure(HttpSecurity http) throws Exception {
        http.httpBasic()
                .and().authorizeRequests()
                    .antMatchers("/admin/**").hasRole("ADMIN")
                    .antMatchers("/api/**", "/images/**", "/logout", "/login", "/login-error", "/").permitAll()
                .and().formLogin()
                    .loginPage("/login")
                    .failureUrl("/login-error")
                    .successForwardUrl("/admin/")
                .and().logout()
                    .logoutSuccessUrl("/login")
                .and()
                    .csrf().disable();
    }

    @Autowired
    public void configure(AuthenticationManagerBuilder auth) throws Exception {
        auth.inMemoryAuthentication().withUser(adminUsername).password("{noop}" + adminPassword).roles("ADMIN");
    }

}